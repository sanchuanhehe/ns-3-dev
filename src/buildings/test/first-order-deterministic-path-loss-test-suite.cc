/*
 * Copyright (c) 2024 Office National d'Etude et de Recherche Aérospatiale (ONERA)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Hugo LE DIRACH  <hugo.le_dirach@onera.fr>
 */

#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/first-order-buildings-aware-propagation-loss-model.h"
#include "ns3/log.h"
#include "ns3/random-variable-stream.h"
#include "ns3/string.h"
#include "ns3/test.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstOrderBuildingsAwarePropagationLossModelTest");

/**
 * @ingroup propagation-tests
 *
 * @brief FirstOrderBuildingsAwarePropagationLossModel Test Case
 *
 */
class FirstOrderBuildingsAwarePropagationLossModelTestCase : public TestCase
{
  public:
    /**
     * Constructor
     */
    FirstOrderBuildingsAwarePropagationLossModelTestCase();

    /**
     * Destructor
     */
    ~FirstOrderBuildingsAwarePropagationLossModelTestCase() override;

  private:
    /**
     * Builds the simulation scenario and perform the tests
     */
    void DoRun() override;
};

FirstOrderBuildingsAwarePropagationLossModelTestCase::
    FirstOrderBuildingsAwarePropagationLossModelTestCase()
    : TestCase("Test case for the class FirstOrderBuildingsAwarePropagationLossModel")
{
}

FirstOrderBuildingsAwarePropagationLossModelTestCase::
    ~FirstOrderBuildingsAwarePropagationLossModelTestCase()
{
}

void
FirstOrderBuildingsAwarePropagationLossModelTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    RngSeedManager::SetSeed(444);
    RngSeedManager::SetRun(1);

    double xMin = 20.0;
    double xMax = 25.0;
    double yMin = 20.0;
    double yMax = 25.0;
    double zMin = 0.0;
    double zMax = 15.0;

    Ptr<Building> building = CreateObject<Building>();
    building->SetBoundaries(Box(xMin, xMax, yMin, yMax, zMin, zMax));
    building->SetBuildingType(Building::Residential);
    building->SetExtWallsType(Building::ConcreteWithWindows);

    Ptr<MobilityModel> txMob = CreateObject<ConstantPositionMobilityModel>();
    Ptr<MobilityModel> rxMob = CreateObject<ConstantPositionMobilityModel>();

    Ptr<FirstOrderBuildingsAwarePropagationLossModel> propagationLossModel =
        CreateObject<FirstOrderBuildingsAwarePropagationLossModel>();

    // test1
    txMob->SetPosition(Vector(15.0, 15.0, 5.0));
    rxMob->SetPosition(Vector(15.0, 30.0, 5.0));
    double lossRef = 62.7633;
    double loss;
    loss = propagationLossModel->GetLoss(txMob, rxMob);
    NS_LOG_INFO("Test 1 : LOS 15m");
    NS_LOG_INFO("Calculated loss: " << loss);
    NS_LOG_INFO("Theoretical loss: " << lossRef);
    NS_TEST_ASSERT_MSG_EQ_TOL(loss, lossRef, 0.1, "test 1 : Wrong loss!");

    // test2
    txMob->SetPosition(Vector(23.0, 15.0, 5.0));
    rxMob->SetPosition(Vector(23.0, 30.0, 5.0));
    lossRef = 121.557;
    loss = propagationLossModel->GetLoss(txMob, rxMob);
    NS_LOG_INFO("Test 2 : NLOS 15m through concrete");
    NS_LOG_INFO("Calculated loss: " << loss);
    NS_LOG_INFO("Theoretical loss: " << lossRef);
    NS_TEST_ASSERT_MSG_EQ_TOL(loss, lossRef, 0.1, "test 2 : Wrong loss!");

    // test3
    txMob->SetPosition(Vector(25.0, 15.0, 5.0));
    rxMob->SetPosition(Vector(15.0, 25.0, 5.0));
    lossRef = 64.9236;
    loss = propagationLossModel->GetLoss(txMob, rxMob);
    NS_LOG_INFO("Test 3 : theta = 0 deg");
    NS_LOG_INFO("Calculated loss: " << loss);
    NS_LOG_INFO("Theoretical loss: " << lossRef);
    NS_TEST_ASSERT_MSG_EQ_TOL(loss, lossRef, 0.1, "test 3 : Wrong loss!");

    // test4
    txMob->SetPosition(Vector(24.0, 15.0, 5.0));
    rxMob->SetPosition(Vector(15.0, 25.0, 5.0));
    lossRef = 60.8709;
    loss = propagationLossModel->GetLoss(txMob, rxMob);
    NS_LOG_INFO("Test 3 : theta = -5 deg");
    NS_LOG_INFO("Calculated loss: " << loss);
    NS_LOG_INFO("Theoretical loss: " << lossRef);
    NS_TEST_ASSERT_MSG_EQ_TOL(loss, lossRef, 0.1, "test 4 : Wrong loss!");

    // test5
    txMob->SetPosition(Vector(26.0, 15.0, 5.0));
    rxMob->SetPosition(Vector(15.0, 25.0, 5.0));
    lossRef = 77.6235;
    loss = propagationLossModel->GetLoss(txMob, rxMob);
    NS_LOG_INFO("Test 3 : theta = 6 deg");
    NS_LOG_INFO("Calculated loss: " << loss);
    NS_LOG_INFO("Theoretical loss: " << lossRef);
    NS_TEST_ASSERT_MSG_EQ_TOL(loss, lossRef, 0.1, "test 5 : Wrong loss!");

    // test6
    txMob->SetPosition(Vector(27.0, 15.0, 5.0));
    rxMob->SetPosition(Vector(15.0, 25.0, 5.0));
    lossRef = 74.1938;
    loss = propagationLossModel->GetLoss(txMob, rxMob);
    NS_LOG_INFO("Test 3 : theta = 9.5 deg");
    NS_LOG_INFO("Calculated loss: " << loss);
    NS_LOG_INFO("Theoretical loss: " << lossRef);
    NS_TEST_ASSERT_MSG_EQ_TOL(loss, lossRef, 0.1, "test 6 : Wrong loss!");

    // test7
    txMob->SetPosition(Vector(29.0, 15.0, 5.0));
    rxMob->SetPosition(Vector(15.0, 25.0, 5.0));
    lossRef = 85.5683;
    loss = propagationLossModel->GetLoss(txMob, rxMob);
    NS_LOG_INFO("Test 3 : theta = 16 deg");
    NS_LOG_INFO("Calculated loss: " << loss);
    NS_LOG_INFO("Theoretical loss: " << lossRef);
    NS_TEST_ASSERT_MSG_EQ_TOL(loss, lossRef, 0.1, "test 7 : Wrong loss!");

    // NS_LOG_INFO("Calculated loss: " << loss);
    // NS_LOG_INFO("Theoretical loss: " << lossRef);
}

/**
 * @ingroup propagation-tests
 *
 * @brief FirstOrderBuildingsAwarePropagationLossModel TestSuite
 *
 */
class FirstOrderBuildingsAwarePropagationLossModelTestSuite : public TestSuite
{
  public:
    FirstOrderBuildingsAwarePropagationLossModelTestSuite();
};

FirstOrderBuildingsAwarePropagationLossModelTestSuite::
    FirstOrderBuildingsAwarePropagationLossModelTestSuite()
    : TestSuite("first-order-buildings-aware-pathloss", Type::SYSTEM)
{
    LogComponentEnable("FirstOrderBuildingsAwarePropagationLossModelTest", LOG_LEVEL_ALL);

    AddTestCase(new FirstOrderBuildingsAwarePropagationLossModelTestCase,
                TestCase::Duration::QUICK);
}

/// Static variable for test initialization
static FirstOrderBuildingsAwarePropagationLossModelTestSuite g_FirstOrderBuildingsAwareTestSuite;
