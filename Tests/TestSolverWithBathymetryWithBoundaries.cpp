/**
 * @file TestSolverWithBathymetryWithBoundaries.cpp
 * contains tests for SolverWithBathymetry
 *
 * @test Ensure that total water volume stays the same with reflecting boundaries on both sides
 *
 */
#include <catch2/catch_test_macros.hpp>

#include "Blocks/WavePropagationBlock.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include "Scenarios/ShockRareProblemScenario.hpp"
#include "Scenarios/SubcriticalFlowScenario.hpp"
#include "Scenarios/SupercriticalFlowScenario.hpp"


TEST_CASE("Ensure that total water volume stays the same with reflecting boundaries on both sides", "Constant total water volume") {
  const unsigned int size = 100;
  const unsigned int time = 200;
  RealType margin, actualWaterVolume;
  const RealType relativeMargin = 0.1;
  RealType* h = new RealType[size + 2];
  RealType* hu = new RealType[size + 2];
  RealType* b = new RealType[size + 2];

  SECTION("supercritical flow scenario") {
    Scenarios::SupercriticalFlowScenario scenario(100);
    RealType totalWaterVolume = 0.0;
    for (unsigned int i = 0; i < size + 2; i++) {
      h[i] = scenario.getHeight(i);
      hu[i] = scenario.getMomentum(i);
      b[i] = scenario.getBathymetry(i);
      totalWaterVolume += scenario.getHeight(i);
    }
    margin = relativeMargin * totalWaterVolume;

    Blocks::WavePropagationBlock wavePropagation(h, hu, b, size, scenario.getCellSize());
    wavePropagation.setLeftBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    wavePropagation.setRightBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    for (unsigned int i = 0; i < time; i++) {
      wavePropagation.applyBoundaryConditions();
      RealType maxTimeStep = wavePropagation.computeNumericalFluxes();
      wavePropagation.updateUnknowns(maxTimeStep);

      actualWaterVolume = 0.0;
      for (unsigned int k = 0; k < size + 2; k++) {
        actualWaterVolume += h[k];
      }
      REQUIRE_THAT(actualWaterVolume, Catch::Matchers::WithinAbs(totalWaterVolume, margin));
    }
  }

  SECTION("subcritical scenario") {
    Scenarios::SubcriticalFlowScenario scenario(100);
    RealType totalWaterVolume = 0.0;
    for (unsigned int i = 0; i < size + 2; i++) {
      h[i] = scenario.getHeight(i);
      hu[i] = scenario.getMomentum(i);
      b[i] = scenario.getBathymetry(i);
      totalWaterVolume += scenario.getHeight(i);
    }
    margin = relativeMargin * totalWaterVolume;

    Blocks::WavePropagationBlock wavePropagation(h, hu, b, size, scenario.getCellSize());
    wavePropagation.setLeftBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    wavePropagation.setRightBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    for (unsigned int i = 0; i < time; i++) {
      wavePropagation.applyBoundaryConditions();
      RealType maxTimeStep = wavePropagation.computeNumericalFluxes();
      wavePropagation.updateUnknowns(maxTimeStep);

      actualWaterVolume = 0.0;
      for (unsigned int k = 0; k < size + 2; k++) {
        actualWaterVolume += h[k];
      }
      REQUIRE_THAT(actualWaterVolume, Catch::Matchers::WithinAbs(totalWaterVolume, margin));
    }
  }

  SECTION("shock-shock scenario") {
    Scenarios::ShockRareProblemScenario scenario(500.0, size, size/3, 200.0, 100.0);
    RealType totalWaterVolume = 0.0;
    for (unsigned int i = 0; i < size + 2; i++) {
      h[i] = scenario.getHeight(i);
      hu[i] = scenario.getMomentum(i);
      b[i] = scenario.getBathymetry(i);
      totalWaterVolume += scenario.getHeight(i);
    }
    margin = relativeMargin * totalWaterVolume;

    Blocks::WavePropagationBlock wavePropagation(h, hu, b, size, scenario.getCellSize());
    wavePropagation.setLeftBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    wavePropagation.setRightBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    for (unsigned int i = 0; i < time; i++) {
      wavePropagation.applyBoundaryConditions();
      RealType maxTimeStep = wavePropagation.computeNumericalFluxes();
      wavePropagation.updateUnknowns(maxTimeStep);

      actualWaterVolume = 0.0;
      for (unsigned int k = 0; k < size + 2; k++) {
        actualWaterVolume += h[k];
      }
      REQUIRE_THAT(actualWaterVolume, Catch::Matchers::WithinAbs(totalWaterVolume, margin));
    }
  }

  SECTION("rare-rare scenario") {
    Scenarios::ShockRareProblemScenario scenario(500.0, size, size/3, 2000.0, -100000.0);
    RealType totalWaterVolume = 0.0;
    for (unsigned int i = 0; i < size + 2; i++) {
      h[i] = scenario.getHeight(i);
      hu[i] = scenario.getMomentum(i);
      b[i] = scenario.getBathymetry(i);
      totalWaterVolume += scenario.getHeight(i);
    }
    margin = relativeMargin * totalWaterVolume;

    Blocks::WavePropagationBlock wavePropagation(h, hu, b, size, scenario.getCellSize());
    wavePropagation.setLeftBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    wavePropagation.setRightBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    for (unsigned int i = 0; i < time; i++) {
      wavePropagation.applyBoundaryConditions();
      RealType maxTimeStep = wavePropagation.computeNumericalFluxes();
      wavePropagation.updateUnknowns(maxTimeStep);

      actualWaterVolume = 0.0;
      for (unsigned int k = 0; k < size + 2; k++) {
        actualWaterVolume += h[k];
      }
      REQUIRE_THAT(actualWaterVolume, Catch::Matchers::WithinAbs(totalWaterVolume, margin));
    }
  }

  SECTION("wavy ground scenario") {

    RealType width = 500.0;
    Scenarios::ShockRareProblemScenario scenario(width, size, size/3, 100.0, 20.0);
    RealType totalWaterVolume = 0.0;
    for (unsigned int i = 0; i < size + 2; i++) {
      unsigned int x = RealType(i % 10 - 5.0) * 5.0;
      RealType b_ = 53.0 - (std::sqrt(50.0*50.0 - x*x));
      h[i] = b_;
      hu[i] = scenario.getMomentum(i);
      b[i] = -b_;
      totalWaterVolume += b_;
    }
    margin = relativeMargin * totalWaterVolume;

    Blocks::WavePropagationBlock wavePropagation(h, hu, b, size, scenario.getCellSize());
    wavePropagation.setLeftBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    wavePropagation.setRightBoundaryCondition(Blocks::WavePropagationBlock::ReflectingBoundary);
    for (unsigned int i = 0; i < time; i++) {
      wavePropagation.applyBoundaryConditions();
      RealType maxTimeStep = wavePropagation.computeNumericalFluxes();
      wavePropagation.updateUnknowns(maxTimeStep);
      actualWaterVolume = 0.0;
      for (unsigned int k = 0; k < size + 2; k++) {
        actualWaterVolume += h[k];
      }
      REQUIRE_THAT(actualWaterVolume, Catch::Matchers::WithinAbs(totalWaterVolume, margin));
    }
  }


  delete[] h;
  delete[] hu;
  delete[] b;
}
