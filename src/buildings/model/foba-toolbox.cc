/*
 * Copyright (c) 2024 Office National d'Etude et de Recherche Aérospatiale (ONERA)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Hugo LE DIRACH  <hugo.le_dirach@onera.fr>
 */

#include "foba-toolbox.h"

#include "building.h"

#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/object.h"
#include "ns3/pointer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("FobaToolBox");

TypeId
FobaToolBox::GetTypeId()
{
    static TypeId tid = TypeId("ns3::FobaToolBox").SetParent<Object>().SetGroupName("Buildings")
        //.AddConstructor<FobaToolBox> ()
        ;
    return tid;
}

TypeId
FobaToolBox::GetInstanceTypeId() const
{
    return FobaToolBox::GetTypeId();
}

FobaToolBox::FobaToolBox()
{
}

FobaToolBox::~FobaToolBox()
{
}

char
FobaToolBox::GetZone(Ptr<MobilityModel> currMob, Ptr<Building> currBuilding)
{
    NS_LOG_FUNCTION(this);

    double currX = currMob->GetPosition().x;
    double currY = currMob->GetPosition().y;

    if (((currX < currBuilding->GetBoundaries().xMax) &&
         (currX > currBuilding->GetBoundaries().xMin)) &&
        ((currY < currBuilding->GetBoundaries().yMax) &&
         (currY > currBuilding->GetBoundaries().yMin)))
    {
        return 'Z'; // Node in walls, since the comparaison is stric, mob on bound is consider
                    // outside
    }

    if (currX <= currBuilding->GetBoundaries().xMin)
    {
        if (currY >= currBuilding->GetBoundaries().yMax)
        {
            return 'A';
        }
        if (currY <= currBuilding->GetBoundaries().yMin)
        {
            return 'G';
        }
        return 'H';
    }
    if (currX >= currBuilding->GetBoundaries().xMax)
    {
        if (currY >= currBuilding->GetBoundaries().yMax)
        {
            return 'C';
        }
        if (currY <= currBuilding->GetBoundaries().yMin)
        {
            return 'E';
        }
        return 'D';
    }
    else
    {
        if (currY >= currBuilding->GetBoundaries().yMax)
        {
            return 'B';
        }
        if (currY <= currBuilding->GetBoundaries().yMin)
        {
            return 'F';
        }
    }
    return 'Z'; // Undefined zone
}

/*
return True for NLOS
*/
bool
FobaToolBox::IsBuildingCausingNlos(Ptr<MobilityModel> firstMob,
                                   Ptr<MobilityModel> secondMob,
                                   Ptr<Building> currBuilding)
{
    NS_LOG_FUNCTION(this);

    double firstMobX = firstMob->GetPosition().x;
    double firstMobY = firstMob->GetPosition().y;
    double firstMobZ = firstMob->GetPosition().z;
    double secondMobX = secondMob->GetPosition().x;
    double secondMobY = secondMob->GetPosition().y;
    double secondMobZ = secondMob->GetPosition().z;

    double buildindXMax = currBuilding->GetBoundaries().xMax;
    double buildindYMax = currBuilding->GetBoundaries().yMax;
    double buildindZMax = currBuilding->GetBoundaries().zMax;
    double buildindXMin = currBuilding->GetBoundaries().xMin;
    double buildindYMin = currBuilding->GetBoundaries().yMin;

    double firstHeightDiff = firstMobZ - buildindZMax;
    double secondHeightDiff = secondMobZ - buildindZMax;

    if ((firstHeightDiff > 0) && (secondHeightDiff > 0))
    { // both node are strictly over roof top height
        return false;
    }
    if (firstHeightDiff * secondHeightDiff <= 0)
    { // one of the node is below or at roof height

        // Check if the line crosses the box on each plan, if so -> NLOS
        // Parameters for the X-Z plan
        double alphaX = (firstMobX - secondMobX) / (firstMobZ - secondMobZ);
        double betaX = firstMobZ - alphaX * firstMobX;
        // Parameters for the Y-Z plan
        double alphaY = (firstMobY - secondMobY) / (firstMobZ - secondMobZ);
        double betaY = firstMobZ - alphaY * firstMobY;
        // Parameters for the X-Y plan
        double alphaZ = (firstMobY - secondMobY) / (firstMobX - secondMobX);
        double betaZ = firstMobY - alphaZ * firstMobX;

        if (((alphaX * buildindXMin - betaX < buildindZMax) ||
             (alphaX * buildindXMax - betaX < buildindZMax)) &&
            ((alphaY * buildindYMin - betaY < buildindZMax) ||
             (alphaY * buildindYMax - betaY < buildindZMax)))
        {
            if (((firstMobX == secondMobX) &&
                 ((firstMobX == buildindXMax) || (firstMobX == buildindXMin))) ||
                ((firstMobY == secondMobY) &&
                 ((firstMobY == buildindYMax) || (firstMobY == buildindYMin))))
            {
                return true;
            }

            if (((alphaZ * buildindXMin - betaZ <= buildindYMax) &&
                 (alphaZ * buildindXMin - betaZ >= buildindYMin)) ||
                ((alphaZ * buildindXMax - betaZ <= buildindYMax) &&
                 (alphaZ * buildindXMax - betaZ >= buildindYMin)))
            {
                return true;
            }
        }
    }
    return false; // LOS by default
}

std::vector<Ptr<Building>>
FobaToolBox::GetBuildingsBetween(Ptr<MobilityModel> firstMob,
                                 Ptr<MobilityModel> secondMob,
                                 std::vector<Ptr<Building>> buildings)
{
    NS_LOG_FUNCTION(this);

    /*
     * Buildings in NS3 are rectangles that are orthogonally aligned with the axis of the
     * environment, taking advantage of this model, we label the area surrounding a building and
     * avoid non-necessary calculation. For example, if the nodes are in respectively zone A and
     * zone G, their specific position does not matter, they will have LOS no matter the shape of
     * the building. However, if the nodes are in zone A and F, we need to evaluate if the link
     * crosses the building, which implies calculations.
     *
     * To assess if we are in a NLOS configuration we take the following steps:
     * 1. Determine the zone
     * 2. Making a quick decision based on default cases
     * 3. If uncertainty persists, compute the linear function between the nodes and check if it
     * crosses the building box
     *
     *      A   |   B    |   C
     *   -------+--------+-------
     *      H   |building|   D
     *   -------+--------+-------
     *      G   |   F    |   E
     */
    // All LOS cases that are automatic LOS --> no assesment needed
    std::vector<std::string> defaultLos = {"AA", "BB", "CC", "DD", "EE", "FF", "GG", "HH",
                                           "AB", "BA", "AC", "CA", "AH", "HA", "BC", "CB",
                                           "CD", "DC", "CE", "EC", "DE", "ED", "EF", "FE",
                                           "EG", "GE", "FG", "GF", "GH", "HG", "AG", "GA"};
    // All LOS cases that are automatic NLOS
    std::vector<std::string> defaultNlos = {"HD", "DH", "BF", "FB"};
    // Zone from which we evaluate the NLOS
    std::vector<char> evaluator = {'A', 'B', 'F', 'G', 'H'};

    char zoneA;
    char zoneB;
    std::string zoneCombin;
    int limit = buildings.size();
    std::vector<Ptr<Building>> nlosBuildings;

    for (int index = 0; index < limit; index++)
    {
        Ptr<Building> building = buildings[index];
        double building_zMax = building->GetBoundaries().zMax;
        double firstMobZ = firstMob->GetPosition().z;
        double secondMobZ = secondMob->GetPosition().z;
        zoneA = GetZone(firstMob, building);
        zoneB = GetZone(secondMob, building);
        zoneCombin = std::string(1, zoneA) + zoneB;
        NS_ASSERT_MSG(zoneA != 'Z', "Undefined zone, check if node is note in the walls");
        NS_ASSERT_MSG(zoneB != 'Z', "Undefined zone, check if node is note in the walls");

        if (std::find(defaultLos.begin(), defaultLos.end(), zoneCombin) != defaultLos.end())
        {
            // We have LOS for this building
            continue;
        }
        if (std::find(defaultNlos.begin(), defaultNlos.end(), zoneCombin) != defaultNlos.end())
        {
            // We have NLOS for this building
            nlosBuildings.push_back(building);
            continue;
        }
        if ((firstMobZ >= building_zMax) && (secondMobZ >= building_zMax))
        {
            // We have LOS for this building
            continue;
        }
        if (std::find(evaluator.begin(), evaluator.end(), zoneA) != evaluator.end())
        {
            if (IsBuildingCausingNlos(firstMob, secondMob, building))
            {
                nlosBuildings.push_back(building);
            }
            continue;
        }
        NS_LOG_DEBUG("Could not asses NLOS");
    }
    return nlosBuildings;
}

std::vector<Vector>
FobaToolBox::GetCorner(Ptr<Building> currBuild, Ptr<MobilityModel> rxMob, Ptr<MobilityModel> txMob)
{
    NS_LOG_FUNCTION(this);

    // Area where the diffraction happens in top left corner
    std::vector<std::string> topLeft = {"BG", "GB", "HB", "BH", "HC", "CH"};
    // Area where the diffraction happens in top right corner
    std::vector<std::string> topRight = {"BE", "EB", "DB", "BD", "DA", "AD"};
    // Area where the diffraction happens in bottom left corner
    std::vector<std::string> botLeft = {"HE", "EH", "FH", "HF", "FA", "AF"};
    // Area where the diffraction happens in bottom left corner
    std::vector<std::string> botRight = {"DG", "GD", "FD", "DF", "FC", "CF"};

    Vector cornerPos;
    std::vector<Vector> corners;
    char zoneA = GetZone(rxMob, currBuild);
    char zoneB = GetZone(txMob, currBuild);
    std::string zoneCombin = std::string(1, zoneA) + zoneB;

    if (std::find(topLeft.begin(), topLeft.end(), zoneCombin) != topLeft.end())
    {
        cornerPos.x = currBuild->GetBoundaries().xMin;
        cornerPos.y = currBuild->GetBoundaries().yMax;
        corners.push_back(cornerPos);
        return corners;
    }
    if (std::find(topRight.begin(), topRight.end(), zoneCombin) != topRight.end())
    {
        cornerPos.x = currBuild->GetBoundaries().xMax;
        cornerPos.y = currBuild->GetBoundaries().yMax;
        corners.push_back(cornerPos);
        return corners;
    }
    if (std::find(botLeft.begin(), botLeft.end(), zoneCombin) != botLeft.end())
    {
        cornerPos.x = currBuild->GetBoundaries().xMin;
        cornerPos.y = currBuild->GetBoundaries().yMin;
        corners.push_back(cornerPos);
        return corners;
    }
    if (std::find(botRight.begin(), botRight.end(), zoneCombin) != botRight.end())
    {
        cornerPos.x = currBuild->GetBoundaries().xMin;
        cornerPos.y = currBuild->GetBoundaries().yMax;
        corners.push_back(cornerPos);
        return corners;
    }
    // Two corners scenario
    if ((zoneCombin == "CG") || (zoneCombin == "GC"))
    {
        Vector secCornerPos;
        cornerPos.x = currBuild->GetBoundaries().xMin;
        cornerPos.y = currBuild->GetBoundaries().yMax;
        secCornerPos.x = currBuild->GetBoundaries().xMax;
        secCornerPos.y = currBuild->GetBoundaries().yMin;
        corners.push_back(cornerPos);
        corners.push_back(secCornerPos);
        return corners;
    }
    // Two corners scenario
    if ((zoneCombin == "AE") || (zoneCombin == "EA"))
    {
        Vector secCornerPos;
        cornerPos.x = currBuild->GetBoundaries().xMin;
        cornerPos.y = currBuild->GetBoundaries().yMin;
        secCornerPos.x = currBuild->GetBoundaries().xMax;
        secCornerPos.y = currBuild->GetBoundaries().yMax;
        corners.push_back(cornerPos);
        corners.push_back(secCornerPos);
        return corners;
    }

    return corners;
}

std::optional<Vector>
FobaToolBox::GetReflectionPoint(Ptr<Building> building,
                                Ptr<MobilityModel> rxMob,
                                Ptr<MobilityModel> txMob)
{
    NS_LOG_FUNCTION(this);

    // yMin areas
    std::vector<std::string> yMin = {"GF", "FG", "FE", "EF", "EG", "GE", "FF"};
    // yMax areas
    std::vector<std::string> yMax = {"AB", "BA", "BC", "CB", "AC", "CA", "BB"};
    // yMin areas
    std::vector<std::string> xMin = {"AH", "HA", "HG", "GH", "GA", "AG", "HH"};
    // yMax areas
    std::vector<std::string> xMax = {"CD", "DC", "DE", "ED", "EC", "CE", "DD"};

    char zoneA = GetZone(rxMob, building);
    char zoneB = GetZone(txMob, building);
    std::string zoneCombin = std::string(1, zoneA) + zoneB;

    if (std::find(yMin.begin(), yMin.end(), zoneCombin) != yMin.end())
    {
        double yRefl = building->GetBoundaries().yMin;
        double rxMobX = rxMob->GetPosition().x;
        double rxMobY = rxMob->GetPosition().y;
        double txMobX = txMob->GetPosition().x;
        double txMobY = txMob->GetPosition().y;
        double xRefl = (rxMobX * (yRefl - txMobY) - txMobX * (rxMobY - yRefl)) /
                       ((yRefl - txMobY) - (rxMobY - yRefl));
        return Vector(xRefl, yRefl, 1);
    }
    if (std::find(yMax.begin(), yMax.end(), zoneCombin) != yMax.end())
    {
        double yRefl = building->GetBoundaries().yMax;
        double rxMobX = rxMob->GetPosition().x;
        double rxMobY = rxMob->GetPosition().y;
        double txMobX = txMob->GetPosition().x;
        double txMobY = txMob->GetPosition().y;
        double xRefl = (rxMobX * (yRefl - txMobY) - txMobX * (rxMobY - yRefl)) /
                       ((yRefl - txMobY) - (rxMobY - yRefl));
        return Vector(xRefl, yRefl, 1);
    }
    if (std::find(xMin.begin(), xMin.end(), zoneCombin) != xMin.end())
    {
        double xRefl = building->GetBoundaries().xMin;
        double rxMobX = rxMob->GetPosition().x;
        double rxMobY = rxMob->GetPosition().y;
        double txMobX = txMob->GetPosition().x;
        double txMobY = txMob->GetPosition().y;
        double yRefl = (rxMobY * (xRefl - txMobX) + txMobY * (xRefl - rxMobX)) /
                       ((xRefl - txMobY) + (xRefl - rxMobX));
        return Vector(xRefl, yRefl, 1);
    }
    if (std::find(xMax.begin(), xMax.end(), zoneCombin) != xMax.end())
    {
        double xRefl = building->GetBoundaries().xMax;
        double rxMobX = rxMob->GetPosition().x;
        double rxMobY = rxMob->GetPosition().y;
        double txMobX = txMob->GetPosition().x;
        double txMobY = txMob->GetPosition().y;
        double yRefl = (rxMobY * (xRefl - txMobX) + txMobY * (xRefl - rxMobX)) /
                       ((xRefl - txMobY) + (xRefl - rxMobX));
        return Vector(xRefl, yRefl, 1);
    }
    return std::nullopt;
}

} // namespace ns3
