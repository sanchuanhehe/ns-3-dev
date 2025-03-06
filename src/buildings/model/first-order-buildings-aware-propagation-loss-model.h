/*
 * Copyright (c) 2024 Office National d'Etude et de Recherche Aérospatiale (ONERA)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Hugo LE DIRACH  <hugo.le_dirach@onera.fr>
 */

#ifndef FIRST_ORDER_DETERMINISTIC_PATHLOSS_H
#define FIRST_ORDER_DETERMINISTIC_PATHLOSS_H

#include "foba-toolbox.h"

#include "ns3/propagation-environment.h"
#include "ns3/propagation-loss-model.h"

namespace ns3
{

class ItuR1411LosPropagationLossModel;

/**
 * @ingroup buildings
 *
 * @brief The FirstOrderBuildingsAwarePropagationLossModel takes into account the presence
 * or not of buildings in the sight of nodes to compute loss. To limit the complexity and
 * computation overhead, we limit the reflection of signal to one and consider only one
 * diffraction effect (thus 'First Order'). Based on dominant path method, we compute only
 * the strongest diffraction and reflection, then use the least loss among LOS, penetration,
 * difraction and reflection to use as loss to apply to the signal.
 *
 * This model differs from HybridBuildingsPropagationLoss by the fact that it consider every
 * buildings in the simulation and takes into account the buildings for outside to outside links.
 *
 * @warning This model is meant to introduce realistic path loss by considering interaction
 * of the signal with buildings, however, it has a level of abstraction, it does not reflect
 * the exact behavior that the signal would have in real life.
 *
 */

class FirstOrderBuildingsAwarePropagationLossModel : public PropagationLossModel
{
  public:
    /**
     * @brief Get the type ID.
     * @return The object TypeId.
     */
    static TypeId GetTypeId();
    FirstOrderBuildingsAwarePropagationLossModel();
    ~FirstOrderBuildingsAwarePropagationLossModel() override;

    /**
     * set the propagation frequency
     *
     * @param freq frequency in GHz
     */
    void SetFrequency(double freq);

    /**
     * set the emitting power
     *
     * @param gain Gain in dBm
     */
    void SetGain(double gain);

    /**
     * @brief Compute the path loss according to the nodes position
     * and the presence or not of buildings in between.
     *
     * @param rxMob the mobility model of the destination
     * @param txMob the mobility model of the source
     * @returns the propagation loss (in dB)
     */
    double GetLoss(Ptr<MobilityModel> rxMob, Ptr<MobilityModel> txMob) const;

  private:
    /**
     * Computes the received power by applying the pathloss model described in
     * 3GPP TR 38.901
     *
     * @param txPowerDbm tx power in dBm
     * @param rxMob tx mobility model
     * @param txMob rx mobility model
     * @return the rx power in dBm
     */
    double DoCalcRxPower(double txPowerDbm,
                         Ptr<MobilityModel> rxMob,
                         Ptr<MobilityModel> txMob) const override;

    /**
     * Assign a fixed random variable stream number to the random variables used by this model.
     *
     * Subclasses must implement this; those not using random variables
     * can return zero.
     *
     * @param stream first stream index to use
     * @return the number of stream indices assigned by this model
     */
    int64_t DoAssignStreams(int64_t stream) override;

    /**
     * @brief Compute the path loss with additionnal loss for all walls traversed.
     *
     * @param nlosBuildings the buildings between the sight of the two nodes
     * @returns the penetration loss (in dB)
     */
    double PenetrationLoss(const std::vector<Ptr<Building>>& nlosBuildings) const;

    /**
     * @brief Compute the path loss that is diffracted by a building with positive angles.
     *
     * Since we only consider one diffraction phenomenon, only one building will be responsible for
     * this type of loss, however, all buildings are evaluated. if there are multiple valide
     * diffraction, only the building that generates the least loss is kept.
     *
     * When assesing the loss, if we accounter a scenario where the the NLOSDiffractionLoss has
     * been called but where there are no valid diffraction, we need to invalidate the result. To
     * do so, we return a +infinity loss value so that when it is compare to penetration and
     * reflection we are sure it wont be selected.
     *
     * @param nlosBuildings the buildings between the sight of the two nodes
     * @param allBuildings List of all the buildings in the simulation
     * @param rxMob the mobility model of the destination
     * @param txMob the mobility model of the source
     * @returns the diffraction loss (in dB)
     */
    double NlosDiffractionLoss(const std::vector<Ptr<Building>>& nlosBuildings,
                               const std::vector<Ptr<Building>>& allBuildings,
                               Ptr<MobilityModel> rxMob,
                               Ptr<MobilityModel> txMob) const;

    /**
     * @brief Compute the path loss that is diffracted by the building(s) with negative angles
     *
     * @param allBuildings the buildings between the sight of the two nodes
     * @param rxMob the mobility model of the destination
     * @param txMob the mobility model of the source
     * @returns the diffraction loss (in dB)
     */
    double LosDiffractionLoss(const std::vector<Ptr<Building>>& allBuildings,
                              Ptr<MobilityModel> rxMob,
                              Ptr<MobilityModel> txMob) const;

    /**
     * @brief Compute the path loss that is reflected on the building(s)
     *
     * @param allBuildings All the buildings in the simulation
     * @param rxMob the mobility model of the destination
     * @param txMob the mobility model of the source
     * @returns the reflection loss (in dB)
     */
    double ReflectionLoss(const std::vector<Ptr<Building>>& allBuildings,
                          Ptr<MobilityModel> rxMob,
                          Ptr<MobilityModel> txMob) const;

    /**
     * @brief Adds noise to the loss, proportionnaly to it's strength
     *
     * @param loss the loss to apply to the signal
     * @returns the propagation loss (in dB)
     */
    double Noise(double loss) const;

    /**
     * @brief Calculate the angle between rxMob, B and txMob on the x-y plan
     *
     * @param rxMob a 3D point
     * @param vectorB a 3D point
     * @param txMob a 3D point
     * @returns The angle (in degrees) between AB and BC
     */
    double CalculateAngle(Ptr<MobilityModel> rxMob, Vector vectorB, Ptr<MobilityModel> txMob) const;

    /**
     * @brief Signal attenuation as a function of the shadowing angle
     *
     * @param angle angle of the shadow between tx, the corner and rx
     * @returns loss (in dB)
     */
    double DiffractionValueCalc(double angle) const;

    /**
     * @brief Get the loss between two node according to ItuR1411
     *
     * @param rxMob the mobility model of the destination
     * @param txMob the mobility model of the source
     * @returns loss (in dB)
     */
    double ItuR1411(Ptr<MobilityModel> rxMob, Ptr<MobilityModel> txMob) const;

    Ptr<ItuR1411LosPropagationLossModel>
        m_ituR1411Los;                   ///< ItuR1411LosPropagationLossModel variable holder
    Ptr<FobaToolBox> m_assess;           ///< FOBA toolbox
    double m_frequency;                  ///< Operating frequency
    double m_txGain;                     ///< Emiting gain
    Ptr<UniformRandomVariable> m_UniRdm; ///< RandomVariable object
};

} // namespace ns3

#endif /* FIRST_ORDER_DETERMINISTIC_PATHLOSS_H */
