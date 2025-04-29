/*
 * Copyright (c) 2024 Office National d'Etude et de Recherche Aérospatiale (ONERA)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Hugo LE DIRACH  <hugo.le_dirach@onera.fr>
 */

#ifndef FOBA_TOOL_BOX_H
#define FOBA_TOOL_BOX_H

#include "building.h"

#include "ns3/mobility-module.h"
#include "ns3/object.h"
#include "ns3/pointer.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>
#include <utility>
#include <vector>

namespace ns3
{

class Building;

/**
 * @brief Assess if the 3D straight line between two points intersect a building.
 *
 * This class provide an optimization tools to make quick decision on Line of
 * Sight (LoS) between two point and a given building, to find the corner of building that causes
 * diffraction and to get the point where the secular reflection occures between two point and a
 * surface.
 *
 * The term quick decision refers to the method where nodes are attributed "zones" relative to there
 * position to buildings and based on combined zones. They may be in LOS by definition and would not
 * need the computation of the linear function and to check if this line intersect the building. In
 * short, this method is able to affirm that nodes are in LOS using only comparators (<,>,>=,<=),
 * but if they are in NLOS, their is an abiguity that is lifted by the computation of the line
 * between the nodes and it's intersection with the building.
 */
class FobaToolBox : public Object
{
  public:
    /**
     * @brief Get the type ID.
     * @return The object TypeId.
     */
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    FobaToolBox();
    ~FobaToolBox() override;

    /**
     * @brief Assesses the number of buildings that cause NLOS.
     *
     * @param firstMob first point of the line to evaluate.
     * @param secondMob second point of the line to evaluate.
     * @param buildings contains the buildings to evaluate.
     * @return the buildings that intersect the line between the two points.
     */
    std::vector<Ptr<Building>> GetBuildingsBetween(Ptr<MobilityModel> firstMob,
                                                   Ptr<MobilityModel> secondMob,
                                                   std::vector<Ptr<Building>> buildings);

    /**
     * @brief Gives the corners that may produce diffraction between Rx and Tx
     *
     * @param currBuild Current Building to evaluate
     * @param rxMob the mobility model of the destination
     * @param txMob the mobility model of the source
     * @return the corners of buildings that may produce a diffraction
     */
    std::vector<Vector> GetCorner(Ptr<Building> currBuild,
                                  Ptr<MobilityModel> rxMob,
                                  Ptr<MobilityModel> txMob);

    /**
     * @brief Gives the corners that may produce reflection between Rx and Tx
     *
     * @param building Current Building to evaluate
     * @param rxMob the mobility model of the destination
     * @param txMob the mobility model of the source
     * @return the coordinates on the surface that may produce a diffraction
     */
    std::optional<Vector> GetReflectionPoint(Ptr<Building> building,
                                             Ptr<MobilityModel> rxMob,
                                             Ptr<MobilityModel> txMob);

  private:
    /** @brief The point is allocated to one of the zone detailed in the figure bellow.
     *
     *        A   |   B    |   C
     *     -------+--------+-------
     *        H   |building|   D
     *     -------+--------+-------
     *        G   |   F    |   E
     *
     * @param currMob point to locate relatively to the building.
     * @param currBuilding building that will be used to categorize the point.
     * @return the zone in which the point belong relatively to the building.
     */
    char GetZone(Ptr<MobilityModel> currMob, Ptr<Building> currBuilding);

    /**
     * @brief Assesses if the building causes NLOS.
     *
     * @param firstMob first point of the line to evaluate.
     * @param secondMob second point of the line to evaluate.
     * @param currBuilding building to evaluate.
     * @return true if the building causes a NLOS, false if there is LOS
     * between eva and ave.
     */
    bool IsBuildingCausingNlos(Ptr<MobilityModel> firstMob,
                               Ptr<MobilityModel> secondMob,
                               Ptr<Building> currBuilding);
};

} // namespace ns3
#endif
