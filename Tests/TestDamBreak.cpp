#include <iostream>
#include <random>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "Scenarios/DamBreakScenario.hpp"
#include "Solver/FWaveSolverV2.hpp"
#include "Blocks/WavePropagationBlock.hpp"


RealType testingValues[3][5] = {
    {10455.23457565769,10455.23457565769,-88.40698975464889,88.40698975464889,10454.95852909595},
    {5301.858210999595,5301.858210999595,-58.83019916774978,58.83019916774978,5301.600254498357},
    {8741.563561672365,8741.563561672365,287.7250122212896,-287.7250122212896,8742.54612546504}
};



TEST_CASE("testing the dam-break scenario", "[DamBreakScenario]"){
    SECTION("Test the Middle State") {
    
        for(int i = 0; i<3; i++){
            double expectedValue = 0;
            int cellsize = 100;
            Scenarios::DamBreakScenario damBreaktest(cellsize, testingValues[i][0], testingValues[i][1], testingValues[i][2], testingValues[i][3]);
        
            int size = 100;

            RealType* h = new RealType[size + 2];
            RealType* hu = new RealType[size + 2];
            for (unsigned int i = 0; i < size + 2; i++) {
                h[i] = damBreaktest.getTestHeight(i);
                hu[i] = damBreaktest.getTestMomentum(i);
            }
            Blocks::WavePropagationBlock wavePropagation(h, hu, size, damBreaktest.getCellSize());
            
            double t = 0;

            for (unsigned int i = 0; i < 100; i++) {
                wavePropagation.setOutflowBoundaryConditions();
                RealType maxTimeStep = wavePropagation.computeNumericalFluxes();
                wavePropagation.updateUnknowns(maxTimeStep);

                t += maxTimeStep;
            }
            std::cout << h[size/2] << std::endl;
            REQUIRE_THAT(testingValues[i][4], Catch::Matchers::WithinAbs(h[size / 2], 0.0001));

            delete[] h;
            delete[] hu;
        }
    }
            
}

