#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Scenarios/DamBreakScenario.hpp"
#include "Solver/FWaveSolverStudent.hpp"
#include "Blocks/WavePropagationBlock.hpp"
#include "FWaveSolver.hpp"
#include <random>
//#include <catch2/catch.hpp>


RealType testingValuesSteady[3][4] = {
    {55.23453057769, 55.23453057769, 105.40693926718389, 105.40693926718389},
    {1501.635899003295, 1501.635899003295, -5.83017498223564, -5.83017498223564},
    {8741.563561672365, 8741.563561672365, 287.7250122212896, 287.7250122212896}
};

RealType testingValues[3][4] = {
    {10455.23457565769,10455.23457565769,-88.40698975464889,88.40698975464889},
    {5301.858210999595,5301.858210999595,-58.83019916774978,58.83019916774978},
    {8741.563561672365,8741.563561672365,287.7250122212896,-287.7250122212896}
};

RealType testingValuesEigenvalues[4][6] = {
    {10, 15, 20, 30, -9.05, 13.05},
    {10, 10, 20, 20, -7.9, 11.9},
    {10, 10, 0, 0, -9.9, 9.9},
    {10, 10.0001, 20, 20.0001, -7.90494096, 11.90494096}
};

RealType testingInputsPositiveEigenvalues[3][4] = {
    {10, 10, 100, 100},
    {5, 5, 50, 50},
    {12, 8, 120, 80}
};

RealType testingInputsNegativeEigenvalues[3][4] = {
    {10, 10, -200, -200},
    {8, 12, -160, -240},
    {15, 15, -300, -300}
};


TEST_CASE("Computing Eigenvalues"){
    Solvers::FWaveSolverStudent our_solver;
    double g = 9.81;
    SECTION("Computing Eigenvalues") {
        RealType eigenvalues[2] = { 0.0, 0.0 };
        our_solver.computeEigenvalues(testingValuesEigenvalues[0][0], testingValuesEigenvalues[0][1], testingValuesEigenvalues[0][2], testingValuesEigenvalues[0][3], eigenvalues);
        REQUIRE_THAT(eigenvalues[0], Catch::Matchers::WithinAbs(testingValuesEigenvalues[0][4], 0.01));
        REQUIRE_THAT(eigenvalues[1], Catch::Matchers::WithinAbs(testingValuesEigenvalues[0][5], 0.01));
    }
    SECTION("Computing Eigenvalues") {
        RealType eigenvalues[2] = { 0.0, 0.0 };
        our_solver.computeEigenvalues(testingValuesEigenvalues[1][0], testingValuesEigenvalues[1][1], testingValuesEigenvalues[1][2], testingValuesEigenvalues[1][3], eigenvalues);
        REQUIRE_THAT(eigenvalues[0], Catch::Matchers::WithinAbs(testingValuesEigenvalues[1][4], 0.01));
        REQUIRE_THAT(eigenvalues[1], Catch::Matchers::WithinAbs(testingValuesEigenvalues[1][5], 0.01));
    }
    SECTION("Computing Eigenvalues") {
        RealType eigenvalues[2] = { 0.0, 0.0 };
        our_solver.computeEigenvalues(testingValuesEigenvalues[2][0], testingValuesEigenvalues[2][1], testingValuesEigenvalues[2][2], testingValuesEigenvalues[2][3], eigenvalues);
        REQUIRE_THAT(eigenvalues[0], Catch::Matchers::WithinAbs(testingValuesEigenvalues[2][4], 0.01));
        REQUIRE_THAT(eigenvalues[1], Catch::Matchers::WithinAbs(testingValuesEigenvalues[2][5], 0.01));
    }
    SECTION("Computing Eigenvalues") {
        RealType eigenvalues[2] = { 0.0, 0.0 };
        our_solver.computeEigenvalues(testingValuesEigenvalues[3][0], testingValuesEigenvalues[3][1], testingValuesEigenvalues[3][2], testingValuesEigenvalues[3][3], eigenvalues);
        REQUIRE_THAT(eigenvalues[0], Catch::Matchers::WithinAbs(testingValuesEigenvalues[3][4], 0.01));
        REQUIRE_THAT(eigenvalues[1], Catch::Matchers::WithinAbs(testingValuesEigenvalues[3][5], 0.01));
    }
}

TEST_CASE("Testing net updates for supersonic problems") {
    SECTION("Net Updates for negative Eigenvalues") {
        for (int i = 0; i < 3; i++)
        {
            Solvers::FWaveSolverStudent our_solver;
            RealType hNetUpdatesLeft;
            RealType hNetUpdatesRight;
            RealType huNetUpdatesLeft;
            RealType huNetUpdatesRight;
            RealType maxEdgeSpeed;
            our_solver.computeNetUpdates(testingInputsNegativeEigenvalues[i][0], testingInputsNegativeEigenvalues[i][1], 
                                            testingInputsNegativeEigenvalues[i][2], testingInputsNegativeEigenvalues[i][3], 0, 0,
                                            hNetUpdatesLeft, hNetUpdatesRight, huNetUpdatesLeft, huNetUpdatesRight, maxEdgeSpeed);

            REQUIRE_THAT(hNetUpdatesRight, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(huNetUpdatesRight, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE(hNetUpdatesLeft != 0);
            REQUIRE(huNetUpdatesLeft != 0);
        }
    }

    SECTION("Net Updates for positive Eigenvalues") {
        for (int i = 0; i < 3; i++)
        {
            Solvers::FWaveSolverStudent our_solver;
            RealType hNetUpdatesLeft;
            RealType hNetUpdatesRight;
            RealType huNetUpdatesLeft;
            RealType huNetUpdatesRight;
            RealType maxEdgeSpeed;
            our_solver.computeNetUpdates(testingInputsPositiveEigenvalues[i][0], testingInputsPositiveEigenvalues[i][1], 
                                            testingInputsPositiveEigenvalues[i][2], testingInputsPositiveEigenvalues[i][3], 0, 0,
                                            hNetUpdatesLeft, hNetUpdatesRight, huNetUpdatesLeft, huNetUpdatesRight, maxEdgeSpeed);

            REQUIRE_THAT(hNetUpdatesLeft, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE_THAT(huNetUpdatesLeft, Catch::Matchers::WithinAbs(0, 0.0001));
            REQUIRE(hNetUpdatesRight != 0);
            REQUIRE(huNetUpdatesRight != 0);
        }
    }

}

TEST_CASE("Testing functionality of our F-Wave-Solver"){

    Solvers::FWaveSolver<RealType> reference_solver;
    Solvers::FWaveSolverStudent our_solver;


    RealType hNetUpdatesLeft;
    RealType hNetUpdatesRight;
    RealType huNetUpdatesLeft;
    RealType huNetUpdatesRight;
    RealType maxEdgeSpeed;

    RealType expected_hNetUpdatesLeft;
    RealType expected_hNetUpdatesRight;
    RealType expected_huNetUpdatesLeft;
    RealType expected_huNetUpdatesRight;
    RealType expected_maxEdgeSpeed;


    SECTION("Net Updates for steady state ql = qr") {
        reference_solver.computeNetUpdates(testingValuesSteady[0][0], testingValuesSteady[0][1], testingValuesSteady[0][2], testingValuesSteady[0][3], 0, 0,
                                            expected_hNetUpdatesLeft, expected_hNetUpdatesRight, expected_huNetUpdatesLeft, expected_huNetUpdatesRight, expected_maxEdgeSpeed);
        our_solver.computeNetUpdates(testingValuesSteady[0][0], testingValuesSteady[0][1], testingValuesSteady[0][2], testingValuesSteady[0][3], 0, 0,
                                            hNetUpdatesLeft, hNetUpdatesRight, huNetUpdatesLeft, huNetUpdatesRight, maxEdgeSpeed);
        REQUIRE_THAT(hNetUpdatesLeft, Catch::Matchers::WithinAbs(expected_hNetUpdatesLeft, 0.0001));
        REQUIRE_THAT(hNetUpdatesRight, Catch::Matchers::WithinAbs(expected_hNetUpdatesRight, 0.0001));
        REQUIRE_THAT(huNetUpdatesLeft, Catch::Matchers::WithinAbs(expected_huNetUpdatesLeft, 0.0001));
        REQUIRE_THAT(huNetUpdatesRight, Catch::Matchers::WithinAbs(expected_huNetUpdatesRight, 0.0001));                    

    }
    
}