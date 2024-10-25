#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>
#include <random>

#include "Blocks/WavePropagationBlock.hpp"
#include "Scenarios/DamBreakScenario.hpp"
#include "Scenarios/ShockRareProblemScenario.hpp"
#include "Solver/FWaveSolverV2.hpp"


RealType testingValues[3][5] = {
  {10455.23457565769,10455.23457565769,-88.40698975464889,88.40698975464889,10454.95852909595},
  {5301.858210999595,5301.858210999595,-58.83019916774978,58.83019916774978,5301.600254498357},
  {8741.563561672365,8741.563561672365,287.7250122212896,-287.7250122212896,8742.54612546504}
};



TEST_CASE("testing the dam-break scenario", "[DamBreakScenario]"){
  SECTION("Test the Middle State") {

    for(int i = 0; i<3; i++){
      double expectedValue = 0;
      int size = 100;
      Scenarios::ShockRareProblemScenario middleStatesTest(1000.0, size, size/2, testingValues[i][0], testingValues[i][2]);

      RealType* h = new RealType[size + 2];
      RealType* hu = new RealType[size + 2];
      for (unsigned int i = 0; i < size + 2; i++) {
        h[i] = middleStatesTest.getHeight(i);
        hu[i] = middleStatesTest.getMomentum(i);
      }
      Blocks::WavePropagationBlock wavePropagation(h, hu, size, middleStatesTest.getCellSize());

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

