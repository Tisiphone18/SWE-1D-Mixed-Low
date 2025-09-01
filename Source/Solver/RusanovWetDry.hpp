/**
* @file RusanovWetDry.hpp
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
  class RusanovWetDry {
  public:
    // XXX Physical constant: gravity
#ifdef SWE_PRIMED_SCALING
    const RealType G = 1.; // (m/s^2)
#else
    const RealType G = 9.81; // (m/s^2)
#endif

    // Depth threshold for "dry" handling (positivity protection)
    explicit RusanovWetDry(RealType h_min_ = RealType(1e-8)) : h_min(h_min_) {}

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
      const RealType& hLTrueValue, const RealType& hRTrueValue,
      const RealType& huLTrueValue, const RealType& huRTrueValue,
      const RealType& bLTrueValue, const RealType& bRTrueValue,
      RealType& hNetUpdateLeft,
      RealType& hNetUpdateRight,
      RealType& huNetUpdateLeft,
      RealType& huNetUpdateRight,
      RealType& maxEdgeSpeed);

    /**
     * @brief Apply reflecting boundary condition when one side is marked "dry" by bathymetry flag.
     */
    void applyBoundaryCondition(
      RealType& hL, RealType& hR,
      RealType& huL, RealType& huR,
      RealType& bL, RealType& bR);

    /// Getter/Setter for the dry threshold
    RealType getHMin() const { return h_min; }
    void setHMin(RealType v) { h_min = v; }

  private:
    RealType h_min; // threshold below which a state is treated as dry
  };
}
