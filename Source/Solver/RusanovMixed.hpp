/**
* @file RusanovMixed.hpp
* @author Lena Elisabeth Holtmannspötter <lena.holtmannspoetter@tum.de>
* Rusanov (local Lax–Friedrichs) solver with hydrostatic reconstruction
* for shallow water equations with wetting & drying.
*
* References:
* - Toro, E. F. (1999). Riemann Solvers and Numerical Methods for Fluid Dynamics (2nd ed.). Springer.
* - Audusse, E., Bouchut, F., Bristeau, M.-O., Klein, R., Perthame, B. (2004).
*   A fast and stable well-balanced scheme with hydrostatic reconstruction for shallow water flows.
 */

#pragma once

#include "Tools/RealType.hpp"

namespace Solvers {
  class RusanovMixed {
  public:
    // XXX Physical constant: gravity
#ifdef SWE_PRIMED_SCALING
    const ComputeType G = 1.; // (m/s^2)
#else
    const ComputeType G = 9.81; // (m/s^2)
#endif

    // Depth threshold for "dry" handling (positivity protection)
    explicit RusanovMixed(RealType h_min_ = RealType(1e-8)) : h_min(h_min_) {}

    /**
     * @brief Rusanov (local Lax–Friedrichs) flux with hydrostatic reconstruction.
     *
     * Computes net updates for height and momentum on each side of an interface.
     * Uses hydrostatic reconstruction (well-balanced & positivity-preserving)
     * and a split-form bed source term.
     *
     * @param[in] hLTrueValue  Water height on left cell center
     * @param[in] hRTrueValue  Water height on right cell center
     * @param[in] huLTrueValue Water momentum on left cell center
     * @param[in] huRTrueValue Water momentum on right cell center
     * @param[in] bLTrueValue  Bathymetry at left cell center
     * @param[in] bRTrueValue  Bathymetry at right cell center
     * @param[out] hNetUpdateLeft   Net update for height to the left cell
     * @param[out] hNetUpdateRight  Net update for height to the right cell
     * @param[out] huNetUpdateLeft  Net update for momentum to the left cell
     * @param[out] huNetUpdateRight Net update for momentum to the right cell
     * @param[out] maxEdgeSpeed     Maximum signal speed at the interface (for CFL)
     */
    void computeNetUpdates(
      const ComputeType& hLTrueValue, const ComputeType& hRTrueValue,
      const ComputeType& huLTrueValue, const ComputeType& huRTrueValue,
      const ComputeType& bLTrueValue, const ComputeType& bRTrueValue,
      ComputeType& hNetUpdateLeft,
      ComputeType& hNetUpdateRight,
      ComputeType& huNetUpdateLeft,
      ComputeType& huNetUpdateRight,
      ComputeType& maxEdgeSpeed);

    /**
     * @brief Apply reflecting boundary condition when one side is marked "dry" by bathymetry flag.
     */
    void applyBoundaryCondition(
      ComputeType& hL, ComputeType& hR,
      ComputeType& huL, ComputeType& huR,
      ComputeType& bL, ComputeType& bR);

    /// Getter/Setter for the dry threshold
    RealType getHMin() const { return h_min; }
    void setHMin(RealType v) { h_min = v; }

  private:
    RealType h_min; // threshold below which a state is treated as dry
  };
}
