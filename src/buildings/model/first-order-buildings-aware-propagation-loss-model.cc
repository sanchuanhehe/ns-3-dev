/*
 * Copyright (c) 2024 Office National d'Etude et de Recherche Aérospatiale (ONERA)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Hugo LE DIRACH  <hugo.le_dirach@onera.fr>
 */

#include "first-order-buildings-aware-propagation-loss-model.h"

#include "building-list.h"
#include "building.h"

#include "ns3/angles.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/node-list.h"
#include "ns3/pointer.h"
#include "ns3/wifi-module.h"
#include "ns3/yans-wifi-phy.h"

// Loss models
#include "ns3/itu-r-1411-los-propagation-loss-model.h"

#include <algorithm>
#include <cmath>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("FirstOrderBuildingsAwarePropagationLossModel");

NS_OBJECT_ENSURE_REGISTERED(FirstOrderBuildingsAwarePropagationLossModel);

FirstOrderBuildingsAwarePropagationLossModel::FirstOrderBuildingsAwarePropagationLossModel()
{
    m_ituR1411Los = CreateObject<ItuR1411LosPropagationLossModel>();
    m_assess = CreateObject<FobaToolBox>();
    m_frequency = 2160e6;
    m_txGain = 25;
    m_UniRdm = CreateObject<UniformRandomVariable>();
}

FirstOrderBuildingsAwarePropagationLossModel::~FirstOrderBuildingsAwarePropagationLossModel()
{
}

TypeId
FirstOrderBuildingsAwarePropagationLossModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::FirstOrderBuildingsAwarePropagationLossModel")
            .SetParent<PropagationLossModel>()
            .AddConstructor<FirstOrderBuildingsAwarePropagationLossModel>()
            .SetGroupName("Propagation")
            .AddAttribute(
                "Frequency",
                "The Frequency  (default is 2.106 GHz).",
                DoubleValue(2160e6),
                MakeDoubleAccessor(&FirstOrderBuildingsAwarePropagationLossModel::SetFrequency),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "TxGain",
                "Emitting Power (default 20 dBm)",
                DoubleValue(20),
                MakeDoubleAccessor(&FirstOrderBuildingsAwarePropagationLossModel::SetGain),
                MakeDoubleChecker<double>());

    return tid;
}

void
FirstOrderBuildingsAwarePropagationLossModel::SetFrequency(double freq)
{
    NS_LOG_FUNCTION(this);

    m_ituR1411Los->SetAttribute("Frequency", DoubleValue(freq));
    m_frequency = freq;
}

void
FirstOrderBuildingsAwarePropagationLossModel::SetGain(double gain)
{
    NS_LOG_FUNCTION(this);
    m_txGain = gain;
}

double
FirstOrderBuildingsAwarePropagationLossModel::GetLoss(Ptr<MobilityModel> rxMob,
                                                      Ptr<MobilityModel> txMob) const
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT_MSG((rxMob->GetPosition().z >= 0) && (txMob->GetPosition().z >= 0),
                  "FirstOrderBuildingsAwarePropagationLossModel does not support underground nodes "
                  "(placed at z < 0)");

    double loss = 0.0;

    // Need to check if both node are outside
    // Need to check if there is at least one building in sim

    // For now singular loss model ITU-R-1411
    loss = ItuR1411(rxMob, txMob);
    NS_LOG_DEBUG("Initial loss (before first order path loss) : " << loss);
    std::vector<Ptr<Building>> nlosBuildings;
    std::vector<Ptr<Building>> allBuildings;
    Vector rxMobPos = rxMob->GetPosition();
    Vector txMobPos = txMob->GetPosition();
    int limit = BuildingList::GetNBuildings();
    for (int index = 0; index < limit; ++index)
    {
        Ptr<Building> currBuilding = BuildingList::GetBuilding(index);
        if (currBuilding->IsIntersect(rxMobPos, txMobPos))
        {
            nlosBuildings.push_back(currBuilding);
        }
        allBuildings.push_back(currBuilding);
    }

    if (!nlosBuildings.empty() && (loss < 90))
    {
        double directPathLoss = loss + PenetrationLoss(nlosBuildings);
        NS_LOG_DEBUG("NLOS first order buildings aware, direct path loss : " << directPathLoss);
        double diffractedPathLoss =
            loss + NlosDiffractionLoss(nlosBuildings, allBuildings, rxMob, txMob);
        NS_LOG_DEBUG(
            "NLOS first order buildings aware, diffracted path loss : " << diffractedPathLoss);
        double reflectedPathLoss = ReflectionLoss(allBuildings, rxMob, txMob);
        NS_LOG_DEBUG(
            "NLOS first order buildings aware, reflected path loss : " << reflectedPathLoss);
        loss = std::min(std::min(directPathLoss, diffractedPathLoss),
                        reflectedPathLoss); // #include <algorithm>
        NS_LOG_INFO(this << "0-0 NLOS first order buildings aware loss : " << loss);
        loss += Noise(loss);
        return loss;
    }
    loss += LosDiffractionLoss(allBuildings, rxMob, txMob);
    NS_LOG_INFO(this << "0-0 LOS first order buildings aware loss : " << loss);
    loss += Noise(loss);
    return loss;
}

double
FirstOrderBuildingsAwarePropagationLossModel::DoCalcRxPower(double txPowerDbm,
                                                            Ptr<MobilityModel> rxMob,
                                                            Ptr<MobilityModel> txMob) const
{
    double rxPow = txPowerDbm;
    rxPow -= GetLoss(rxMob, txMob);
    return rxPow;
}

int64_t
FirstOrderBuildingsAwarePropagationLossModel::DoAssignStreams(int64_t stream)
{
    NS_LOG_FUNCTION(this);

    m_UniRdm->SetStream(stream);

    return 1;
}

double
FirstOrderBuildingsAwarePropagationLossModel::PenetrationLoss(
    const std::vector<Ptr<Building>>& nlosBuildings) const
{
    NS_LOG_FUNCTION(this);

    double loss = 0;
    for (size_t i = 0; i < nlosBuildings.size(); ++i)
    {
        if (nlosBuildings[i]->GetExtWallsType() == Building::Wood)
        {
            loss += 2 * 20;
        }
        if (nlosBuildings[i]->GetExtWallsType() == Building::ConcreteWithWindows)
        {
            loss += 2 * 30;
        }
        if (nlosBuildings[i]->GetExtWallsType() == Building::ConcreteWithoutWindows)
        {
            loss += 2 * 30;
        }
        if (nlosBuildings[i]->GetExtWallsType() == Building::StoneBlocks)
        {
            loss += 2 * 40;
        }
        else
        {
            NS_LOG_ERROR(this << " Unknown Wall Type");
        }
    }
    return loss;
}

double
FirstOrderBuildingsAwarePropagationLossModel::NlosDiffractionLoss(
    const std::vector<Ptr<Building>>& nlosBuildings,
    const std::vector<Ptr<Building>>& allBuildings,
    Ptr<MobilityModel> rxMob,
    Ptr<MobilityModel> txMob) const
{
    NS_LOG_FUNCTION(this);

    auto createTempMobilityModel = [](Vector position) {
        Ptr<ConstantPositionMobilityModel> tempMobility =
            CreateObject<ConstantPositionMobilityModel>();
        tempMobility->SetPosition(position);
        return tempMobility;
    };

    for (size_t i = 0; i < nlosBuildings.size(); ++i)
    {
        std::vector<Vector> cornersPos = m_assess->GetCorner(nlosBuildings[i], rxMob, txMob);
        const size_t sizeCornersPos = cornersPos.size();
        if (sizeCornersPos == 1)
        {
            Ptr<MobilityModel> cornerPos = createTempMobilityModel(cornersPos[0]);
            std::vector<Ptr<Building>> nlosCorner =
                m_assess->GetBuildingsBetween(cornerPos, txMob, allBuildings);
            if (nlosCorner.empty())
            {
                double theta = CalculateAngle(txMob, cornersPos[0], rxMob);
                NS_LOG_DEBUG("NLOS diffraction, theta : " << theta << " on corner "
                                                          << cornersPos[0]);
                return DiffractionValueCalc(theta);
            }
        }
        if (sizeCornersPos == 2)
        {
            Ptr<MobilityModel> firstCornerPos = createTempMobilityModel(cornersPos[0]);
            Ptr<MobilityModel> secondCornerPos = createTempMobilityModel(cornersPos[1]);
            std::vector<Ptr<Building>> firstNlosCorner =
                m_assess->GetBuildingsBetween(firstCornerPos, txMob, allBuildings);
            std::vector<Ptr<Building>> secondNlosCorner =
                m_assess->GetBuildingsBetween(secondCornerPos, txMob, allBuildings);
            if (firstNlosCorner.empty() || secondNlosCorner.empty())
            {
                double firstTheta = CalculateAngle(txMob, cornersPos[0], rxMob);
                double secondTheta = CalculateAngle(txMob, cornersPos[1], rxMob);
                NS_LOG_DEBUG("NLOS diffraction, firstTheta : "
                             << firstTheta << " on corner " << cornersPos[0]
                             << " secondTheta : " << secondTheta << " on corner " << cornersPos[1]);
                double firstLoss = DiffractionValueCalc(firstTheta);
                double secondLoss = DiffractionValueCalc(secondTheta);
                return std::min(firstLoss, secondLoss);
            }
        }
        if (sizeCornersPos > 2)
        {
            NS_LOG_ERROR(this << "Unexpected amount of corners");
            return std::numeric_limits<double>::infinity();
            ;
        }
    }
    return std::numeric_limits<double>::infinity();
}

double
FirstOrderBuildingsAwarePropagationLossModel::LosDiffractionLoss(
    const std::vector<Ptr<Building>>& allBuildings,
    Ptr<MobilityModel> rxMob,
    Ptr<MobilityModel> txMob) const
{
    NS_LOG_FUNCTION(this);

    auto createTempMobilityModel = [](Vector position) {
        Ptr<ConstantPositionMobilityModel> tempMobility =
            CreateObject<ConstantPositionMobilityModel>();
        tempMobility->SetPosition(position);
        return tempMobility;
    };

    std::vector<double> losses;
    for (size_t j = 0; j < allBuildings.size(); ++j)
    {
        std::vector<Vector> cornersPos = m_assess->GetCorner(allBuildings[j], rxMob, txMob);
        const size_t sizeCornersPos = cornersPos.size();
        if (sizeCornersPos == 1)
        {
            Ptr<MobilityModel> cornerPos = createTempMobilityModel(cornersPos[0]);
            std::vector<Ptr<Building>> nlosCorner =
                m_assess->GetBuildingsBetween(cornerPos, txMob, allBuildings);
            if (nlosCorner.empty())
            {
                double theta = -CalculateAngle(txMob, cornersPos[0], rxMob);
                NS_LOG_DEBUG("NLOS diffraction, theta : " << theta << " on corner "
                                                          << cornersPos[0]);
                losses.push_back(DiffractionValueCalc(theta));
            }
        }
        if (sizeCornersPos > 1)
        {
            NS_LOG_ERROR(
                this
                << "In LOS, a given building should at most be source of one (1) difffraction");
            return 0.0;
            ;
        }
    }

    if (!losses.empty())
    {
        auto maxIt =
            std::max_element(losses.begin(), losses.end()); // Get iterator to the maximum element
        double maxL = *maxIt; // Dereference the iterator to get the value
        if (maxL >= 0)
        {
            return maxL;
        }
        return 0.0;
    }
    return 0.0;
}

double
FirstOrderBuildingsAwarePropagationLossModel::ReflectionLoss(
    const std::vector<Ptr<Building>>& allBuildings,
    Ptr<MobilityModel> rxMob,
    Ptr<MobilityModel> txMob) const
{
    NS_LOG_FUNCTION(this);

    std::vector<double> reflLoss;
    double reflCoef = 0;

    auto createTempMobilityModel = [](Vector position) {
        Ptr<ConstantPositionMobilityModel> tempMobility =
            CreateObject<ConstantPositionMobilityModel>();
        tempMobility->SetPosition(position);
        return tempMobility;
    };

    for (size_t i = 0; i < allBuildings.size(); ++i)
    {
        std::optional<Vector> reflectionPoint =
            m_assess->GetReflectionPoint(allBuildings[i], rxMob, txMob);
        if (reflectionPoint)
        {
            // Check if NLOS conditions are met
            Ptr<MobilityModel> reflectionMobility = createTempMobilityModel(*reflectionPoint);
            if (m_assess->GetBuildingsBetween(reflectionMobility, rxMob, {allBuildings[i]})
                    .empty() &&
                m_assess->GetBuildingsBetween(reflectionMobility, txMob, {allBuildings[i]}).empty())
            {
                // Assign reflection coefficient based on wall type
                switch (allBuildings[i]->GetExtWallsType())
                {
                case Building::Wood:
                    reflCoef = 0.4;
                    break;
                case Building::ConcreteWithWindows:
                    reflCoef = 0.6;
                    break;
                case Building::ConcreteWithoutWindows:
                    reflCoef = 0.61;
                    break;
                case Building::StoneBlocks:
                    reflCoef = 0.9;
                    break;
                default:
                    NS_LOG_ERROR(this << " Unknown Wall Type");
                    continue;
                }
                // Calculate loss
                NS_LOG_DEBUG("NLOS reflection at : " << *reflectionPoint
                                                     << " txMob-reflection-point loss : "
                                                     << ItuR1411(txMob, reflectionMobility)
                                                     << " reflection-point-rxMob loss : "
                                                     << ItuR1411(reflectionMobility, rxMob));
                // Calculate the 'first half'
                double firstHalf = m_txGain - ItuR1411(txMob, reflectionMobility);
                // Apply attenuation coefficient
                double rxGain =
                    (firstHalf > 0)
                        ? (firstHalf * reflCoef - ItuR1411(reflectionMobility, rxMob))
                        : (firstHalf * (1 + (1 - reflCoef)) - ItuR1411(reflectionMobility, rxMob));
                double loss = m_txGain - rxGain;
                // double loss =
                //     ItuR1411(tx, reflectionMobility) + ItuR1411(reflectionMobility, rx) +
                //     reflCoef;
                reflLoss.push_back(loss);
            }
        }
    }

    // Return the minimum reflection loss
    if (!reflLoss.empty())
    {
        return *std::min_element(reflLoss.begin(), reflLoss.end());
    }
    else
    {
        return std::numeric_limits<double>::infinity(); // No valid reflections
    }
}

double
FirstOrderBuildingsAwarePropagationLossModel::Noise(double loss) const
{
    NS_LOG_FUNCTION(this);

    double y = 0.25 * loss + 5;
    double top = y * 1.1;
    double bot = y * (1 - .1);
    double borne = std::abs(top - bot);
    m_UniRdm->SetAttribute("Min", DoubleValue(-borne));
    m_UniRdm->SetAttribute("Max", DoubleValue(+borne));
    return m_UniRdm->GetValue();
}

double
FirstOrderBuildingsAwarePropagationLossModel::CalculateAngle(Ptr<MobilityModel> rxMob,
                                                             Vector vectorB,
                                                             Ptr<MobilityModel> txMob) const
{ // Test available at Angletest.cc
    Vector A = rxMob->GetPosition();
    Vector C = txMob->GetPosition();

    Vector2D AB(vectorB.x - A.x, vectorB.y - A.y);
    Vector2D BC(C.x - vectorB.x, C.y - vectorB.y);

    double cosTheta = (AB * BC) / (AB.GetLength() * BC.GetLength());

    return RadiansToDegrees(std::acos(cosTheta));
}

double
FirstOrderBuildingsAwarePropagationLossModel::DiffractionValueCalc(double angle) const
{
    NS_LOG_FUNCTION(this);

    double param1 = 0.70;
    double param2 = 24.9;
    double param3 = 3.555;
    double param4 = 31.7;
    return -param1 / (exp((angle / param2) - param3)) + param4;
}

double
FirstOrderBuildingsAwarePropagationLossModel::ItuR1411(Ptr<MobilityModel> rxMob,
                                                       Ptr<MobilityModel> txMob) const
{
    NS_LOG_FUNCTION(this);

    return m_ituR1411Los->GetLoss(rxMob, txMob);
}

} // namespace ns3
