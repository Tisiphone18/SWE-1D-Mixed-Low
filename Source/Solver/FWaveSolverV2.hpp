#ifndef FWAVESOLVERV2_H
#define FWAVESOLVERV2_H

#include "Tools/RealType.hpp"

namespace Solvers {
  class FWaveSolverStudentV2 {
  public:
    // Physical constant: gravity
    const RealType G = 9.81; // in (m/s^2)

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
      const RealType& hL, const RealType& hR,
      const RealType& huL, const RealType& huR,
      const RealType& bL, const RealType& bR,
      RealType& hNetUpdateLeft,
      RealType& hNetUpdateRight,
      RealType& huNetUpdateLeft,
      RealType& huNetUpdateRight,
      RealType& maxEdgeSpeed);

  private:
    /**
     * Compute the eigenvalues (wave speeds)
     *
     * @param hL Water height on left side of edge
     * @param hR Water height on right side of edge
     * @param huL Water momentum on left side of edge
     * @param huR Water momentum on right side of edge
     * @param eigenvalues will be set to: eigenvalues/wave speeds.
     */
    void computeEigenvalues(RealType hL, RealType hR, RealType huL, RealType huR, RealType eigenvalues[2]);
  };
}

#endif // FWAVESOLVERV2_H
