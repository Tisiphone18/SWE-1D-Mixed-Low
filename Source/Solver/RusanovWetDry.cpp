/**
* @file RusanovWetDry.cpp
* Implementation of Rusanov (LLF) solver with hydrostatic reconstruction
* for wetting & drying in shallow water equations.
 */

#include "RusanovWetDry.hpp"

#include <cmath>
#include <iostream>

#include "Tools/RealMath.hpp" // sqrt_real, abs_real, max_real, min_real

namespace Solvers {

  void RusanovWetDry::computeNetUpdates(
    const RealType& hLTrueValue, const RealType& hRTrueValue,
    const RealType& huLTrueValue, const RealType& huRTrueValue,
    const RealType& bLTrueValue, const RealType& bRTrueValue,
    RealType& hNetUpdateLeft,
    RealType& hNetUpdateRight,
    RealType& huNetUpdateLeft,
    RealType& huNetUpdateRight,
    RealType& maxEdgeSpeed)
  {
    // Local copies
    RealType hL  = hLTrueValue;
    RealType hR  = hRTrueValue;
    RealType huL = huLTrueValue;
    RealType huR = huRTrueValue;
    RealType bL  = bLTrueValue;
    RealType bR  = bRTrueValue;

    // Reflective/dry boundary handling
    applyBoundaryCondition(hL, hR, huL, huR, bL, bR);

    // If both sides "dry" -> no updates
    if (bL >= 0.0 && bR >= 0.0) {
      hNetUpdateLeft = hNetUpdateRight = huNetUpdateLeft = huNetUpdateRight = 0.0;
      maxEdgeSpeed = 0.0;
      return;
    }

    // tiny depths: treat as dry
    auto sanitize = [&](RealType& h, RealType& hu) {
      if (h < h_min) { h = RealType(0); hu = RealType(0); }
    };
    sanitize(hL, huL);
    sanitize(hR, huR);

    // Hydrostatic reconstruction (Audusse et al. 2004)
    // Clip each water column against the higher bed at the interface.
    const RealType bmax   = max_real(bL, bR);
    const RealType hLstar = max_real(RealType(0), hL + (bL - bmax));
    const RealType hRstar = max_real(RealType(0), hR + (bR - bmax));

    // Scale momentum consistently with reconstructed depth
    const RealType huLstar = (hLstar > RealType(0) && hL > RealType(0))
                               ? huL * (hLstar / hL) : RealType(0);
    const RealType huRstar = (hRstar > RealType(0) && hR > RealType(0))
                               ? huR * (hRstar / hR) : RealType(0);

    // If both reconstructed sides are dry → no flux, no source
    if (hLstar <= RealType(0) && hRstar <= RealType(0)) {
      hNetUpdateLeft = hNetUpdateRight = huNetUpdateLeft = huNetUpdateRight = 0.0;
      maxEdgeSpeed = 0.0;
      return;
    }

    // Velocities and wave speeds from reconstructed states
    const RealType uL = (hLstar > RealType(0)) ? (huLstar / hLstar) : RealType(0);
    const RealType uR = (hRstar > RealType(0)) ? (huRstar / hRstar) : RealType(0);
    const RealType cL = sqrt_real(G * hLstar);
    const RealType cR = sqrt_real(G * hRstar);

    // Rusanov alpha = max(|u| + c)
    const RealType alpha = max_real(abs_real(uL) + cL, abs_real(uR) + cR);

    // Physical fluxes from reconstructed states
    const RealType fL_h  = huLstar;
    const RealType fL_hu = huLstar * uL + (RealType(0.5) * G) * hLstar * hLstar;
    const RealType fR_h  = huRstar;
    const RealType fR_hu = huRstar * uR + (RealType(0.5) * G) * hRstar * hRstar;

    // Rusanov (LLF) numerical flux
    const RealType hFlux  = RealType(0.5) * (fL_h  + fR_h )
                           - RealType(0.5) * alpha * (hRstar  - hLstar);
    const RealType huFlux = RealType(0.5) * (fL_hu + fR_hu)
                            - RealType(0.5) * alpha * (huRstar - huLstar);

    // Well-balanced bed source term (split form) using reconstructed depths
    const RealType psi = -RealType(0.5) * G * (hLstar + hRstar) * (bR - bL);

    // Net updates (left gets +flux, right gets -flux), add bed split
    hNetUpdateLeft   =  hFlux;
    huNetUpdateLeft  =  huFlux - RealType(0.5) * psi;
    hNetUpdateRight  = -hFlux;
    huNetUpdateRight = -huFlux - RealType(0.5) * psi;

    // CFL edge speed
    maxEdgeSpeed = alpha;

#ifdef DEBUG
    std::cout << "RusanovWetDry:\n"
              << "  hL=" << static_cast<float>(hL) << ", huL=" << static_cast<float>(huL) << ", bL=" << static_cast<float>(bL) << "\n"
              << "  hR=" << static_cast<float>(hR) << ", huR=" << static_cast<float>(huR) << ", bR=" << static_cast<float>(bR) << "\n"
              << "  hL*=" << static_cast<float>(hLstar) << ", huL*=" << static_cast<float>(huLstar)
              << ", hR*=" << static_cast<float>(hRstar) << ", huR*=" << static_cast<float>(huRstar) << "\n"
              << "  uL=" << static_cast<float>(uL) << ", uR=" << static_cast<float>(uR)
              << ", cL=" << static_cast<float>(cL) << ", cR=" << static_cast<float>(cR) << ", alpha=" << static_cast<float>(alpha) << "\n"
              << "  hFlux=" << static_cast<float>(hFlux) << ", huFlux=" << static_cast<float>(huFlux) << ", psi=" << static_cast<float>(psi) << "\n"
              << "  hΔL=" << static_cast<float>(hNetUpdateLeft) << ", hΔR=" << static_cast<float>(hNetUpdateRight) << "\n"
              << "  huΔL=" << static_cast<float>(huNetUpdateLeft) << ", huΔR=" << static_cast<float>(huNetUpdateRight) << "\n"
              << "  maxEdgeSpeed=" << static_cast<float>(maxEdgeSpeed) << "\n\n";
#endif
  }

  void RusanovWetDry::applyBoundaryCondition(
    RealType& hL, RealType& hR,
    RealType& huL, RealType& huR,
    RealType& bL, RealType& bR)
  {
    if (bL >= 0.0) {
      hL  = hR;
      huL = -huR;
      bL  = bR;
    } else if (bR >= 0.0) {
      hR  = hL;
      huR = -huL;
      bR  = bL;
    }
  }

} // namespace Solvers
