#ifndef FWAVESOLVERV2_H
#define FWAVESOLVERV2_H

#include "Tools/RealType.hpp"

namespace Solvers {
  class FWaveSolverStudentV2 {
  public:

/**
 * @brief Implementation of the F-Wave Solver.
 *
 * This function calculates the net updates for the height and momentum
 * on the left and right sides of the edge using the F-Wave approach.
 *
 * @param[in] hL Water height on the left side of the edge.
 * @param[in] hR Water height on the right side of the edge.
 * @param[in] huL Water momentum on the left side of the edge.
 * @param[in] huR Water momentum on the right side of the edge.
 * @param[in] bL Bathymetry value on the left side of the edge.
 * @param[in] bR Bathymetry value on the right side of the edge.
 * @param[out] hNetUpdateLeft Net update for water height to the left cell.
 * @param[out] hNetUpdateRight Net update for water height to the right cell.
 * @param[out] huNetUpdateLeft Net update for momentum to the left cell.
 * @param[out] huNetUpdateRight Net update for momentum to the right cell.
 * @param[out] maxEdgeSpeed The maximum wave speed across the edge.
 */

    void computeNetUpdates(
      const RealType &hL, const RealType &hR,
      const RealType &huL, const RealType &huR,
      const RealType &bL, const RealType &bR,
      RealType& hNetUpdateLeft,
      RealType& hNetUpdateRight,
      RealType& huNetUpdateLeft,
      RealType& huNetUpdateRight,
      RealType& maxEdgeSpeed);
  };
}

#endif // FWAVESOLVERV2_H
