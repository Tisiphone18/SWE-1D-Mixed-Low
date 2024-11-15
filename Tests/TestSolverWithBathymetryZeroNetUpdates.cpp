/**
 * @file TestSolverWithBathymetryZeroNetUpdates.cpp
 * contains tests for SolverWithBathymetry
 *
 * @test Zero net updates in case of changing bathymetry, but "flat" water level and zero momentum
 *
 */
#include <catch2/catch_test_macros.hpp>

#include "Blocks/WavePropagationBlock.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"


TEST_CASE("Zero net updates in case of flat water level", "Zero Net Updates") {
  const unsigned int size = 1000;
  RealType margin = 0.000001;
  RealType* h = new RealType[size + 2];
  RealType* hu = new RealType[size + 2];
  RealType* b = new RealType[size + 2];

  SECTION("Zero Net Updates") {
    std::srand(1234);

    RealType maxDepth = 0.0;
    for (unsigned int i = 0; i < size + 2; i++) {
      RealType h_ = std::rand() % 10000 + 1;
      h[i] = h_;
      hu[i] = 0.0;
      b[i] = -h_;
      maxDepth = std::min(maxDepth, -h_);
    }

    Solvers::FWaveSolverStudentWithBathymetry solver;
    RealType hLNet, hRNet, huLNet, huRNet, xy;
    for (unsigned int i = 1; i < size + 1; i++) {
      solver.computeNetUpdates(h[i - 1], h[i], hu[i - 1], hu[i], b[i - 1], b[i], hLNet, hRNet, huLNet, huRNet, xy);
    }
    REQUIRE_THAT(hLNet, Catch::Matchers::WithinAbs(0.0, margin));
    REQUIRE_THAT(hRNet, Catch::Matchers::WithinAbs(0.0, margin));
    REQUIRE_THAT(huLNet, Catch::Matchers::WithinAbs(0.0, margin));
    REQUIRE_THAT(huRNet, Catch::Matchers::WithinAbs(0.0, margin));
  }

  delete[] h;
  delete[] hu;
  delete[] b;
}
