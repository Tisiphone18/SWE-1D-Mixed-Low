/**
 * @file TestMiddleStatesSolverWithBathymetry.cpp
 * contains tests for SolverWithBathymetry
 *
 * @test Test using middle states from assignment 1 with constant/flat bathymetry should not affect simulation outcome
 *
 */
#include <catch2/catch_test_macros.hpp>

#include "Blocks/WavePropagationBlock.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

unsigned int numOfMiddleStates = 10;
inline RealType testingValues[10][5] = {
  {	8027.37052989784	,	8027.37052989784	,	-673.421223876821	,	673.421223876821	,	8024.97096123124	}	,
  {	4644.31532472814	,	4644.31532472814	,	-389.718000793825	,	389.718000793825	,	4642.48969614061	}	,
  {	4605.06280866981	,	4605.06280866981	,	-381.275290617638	,	381.275290617638	,	4603.26913231775	}	,
  {	6946.8507755619	,	6946.8507755619	,	636.353083309348	,	-636.353083309348	,	6949.28862991558	}	,
  {	5363.16012540991	,	5363.16012540991	,	259.390678929799	,	-259.390678929799	,	5364.29104667798	}	,
  {	1644.43072715937	,	1957.22266657384	,	152.35181956124	,	697.728467320789	,	1795.58340856224	}	,
  {	6144.0765190046	,	6341.50227127248	,	379.973506477461	,	96.8711176387911	,	6242.98533220252	}	,
  {	7161.43689029458	,	6930.88985011389	,	251.03383395008	,	100.175991745293	,	7045.96644619481	}	,
  {	7876.22578338104	,	7965.8070961724	,	193.45847021197	,	165.196027465718	,	7921.00738582347	}	,
  {	7735.56172051487	,	8068.60880432805	,	-720.375168070586	,	-701.44794772533	,	7901.11653212599	}	,
  };


TEST_CASE("Test middle states for constant seafloor", "Middle States") {
  const unsigned int size = 100;
  const unsigned int time = 200;
  RealType margin;
  const RealType relativeMargin = 0.001;
  RealType* h = new RealType[size + 2];
  RealType* hu = new RealType[size + 2];
  RealType* b = new RealType[size + 2];

  SECTION("Middle States") {
    for (unsigned int x = 0; x < numOfMiddleStates; x++) {
      RealType maxDepth = -std::max(testingValues[x][0], testingValues[x][1]);
      for (unsigned int i = 0; i < size + 2; i++) {
        if (i <= size/2) {
          h[i] = testingValues[x][0];
          hu[i] = testingValues[x][2];
        } else {
          h[i] = testingValues[x][1];
          hu[i] = testingValues[x][3];
        }
        b[i] = maxDepth;
      }
      margin = relativeMargin * std::fabs((-maxDepth - testingValues[x][4]));

      Blocks::WavePropagationBlock wavePropagation(h, hu, b, size, 1000.0/size);
      for (unsigned int i = 0; i < time; i++) {
        wavePropagation.applyBoundaryConditions();
        RealType maxTimeStep = wavePropagation.computeNumericalFluxes();
        wavePropagation.updateUnknowns(maxTimeStep);
      }
      REQUIRE_THAT(h[size/2], Catch::Matchers::WithinAbs(testingValues[x][4], margin));
    }
  }


  delete[] h;
  delete[] hu;
  delete[] b;
}