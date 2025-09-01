/**
* @file RusanovMixed.cpp
* Implementation of Rusanov (LLF) solver with hydrostatic reconstruction
* for wetting & drying in shallow water equations.
 */

#include "RusanovMixed.hpp"

#include <cmath>
#include <iostream>

#include "Tools/RealMath.hpp" // sqrt_real, abs_real, max_real, min_real

namespace Solvers {

  void RusanovMixed::computeNetUpdates(
    const ComputeType& hLTrueValue, const ComputeType& hRTrueValue,
    const ComputeType& huLTrueValue, const ComputeType& huRTrueValue,
    const ComputeType& bLTrueValue, const ComputeType& bRTrueValue,
    ComputeType& hNetUpdateLeft,
    ComputeType& hNetUpdateRight,
    ComputeType& huNetUpdateLeft,
    ComputeType& huNetUpdateRight,
    ComputeType& maxEdgeSpeed)
  {
    //std::cout << "RusanovMixed:\n";

    // Promote to compute precision and save as local copies
    ComputeType hL  = static_cast<ComputeType>(hLTrueValue);
    ComputeType hR  = static_cast<ComputeType>(hRTrueValue);
    ComputeType huL = static_cast<ComputeType>(huLTrueValue);
    ComputeType huR = static_cast<ComputeType>(huRTrueValue);
    ComputeType bL  = bLTrueValue;
    ComputeType bR  = bRTrueValue;

    // Reflective/dry boundary handling
    applyBoundaryCondition(hL, hR, huL, huR, bL, bR);

    // TODO tol
    // If both sides "dry" -> no updates
    //if (bL >= 0.0 && bR >= 0.0) {
    //  hNetUpdateLeft = hNetUpdateRight = huNetUpdateLeft = huNetUpdateRight = ComputeType(0.0);
    //  maxEdgeSpeed = ComputeType(0.0);
    //  return;
    //}

    // tiny depths: treat as dry
    auto sanitize = [&](ComputeType& h, ComputeType& hu) {
      if (h < h_min) { h = ComputeType(0); hu = ComputeType(0); }
    };
    sanitize(hL, huL);
    sanitize(hR, huR);

    // Hydrostatic reconstruction (Audusse et al. 2004)
    const ComputeType bmax   = max_compute(bL, bR);
    const ComputeType hLstar = max_compute(ComputeType(0), hL + (bL - bmax));
    const ComputeType hRstar = max_compute(ComputeType(0), hR + (bR - bmax));

    // Scale momentum consistently with reconstructed depth
    const ComputeType huLstar = (hLstar > ComputeType(0) && hL > ComputeType(0))
                               ? huL * (hLstar / hL) : ComputeType(0);
    const ComputeType huRstar = (hRstar > ComputeType(0) && hR > ComputeType(0))
                               ? huR * (hRstar / hR) : ComputeType(0);

    // If both reconstructed sides are dry → no flux, no source
    if (hLstar <= ComputeType(0) && hRstar <= ComputeType(0)) {
      hNetUpdateLeft = hNetUpdateRight = huNetUpdateLeft = huNetUpdateRight = 0.0;
      maxEdgeSpeed = 0.0;
      return;
    }

    // TODO tol
    // Velocities and wave speeds from reconstructed states
    const ComputeType uL = (hLstar > ComputeType(0)) ? (huLstar / hLstar) : ComputeType(0);
    const ComputeType uR = (hRstar > ComputeType(0)) ? (huRstar / hRstar) : ComputeType(0);
    const ComputeType cL = sqrt_compute(G * hLstar);
    const ComputeType cR = sqrt_compute(G * hRstar);

    // Rusanov alpha = max(|u| + c)
    const ComputeType alpha = max_compute(abs_compute(uL) + cL, abs_compute(uR) + cR);

    // Physical fluxes from reconstructed states
    const ComputeType fL_h  = huLstar;
    const ComputeType fL_hu = huLstar * uL + (ComputeType(0.5) * G) * hLstar * hLstar;
    const ComputeType fR_h  = huRstar;
    const ComputeType fR_hu = huRstar * uR + (ComputeType(0.5) * G) * hRstar * hRstar;

    // Rusanov (LLF) numerical flux
    const ComputeType hFlux  = ComputeType(0.5) * (fL_h  + fR_h )
                           - ComputeType(0.5) * alpha * (hRstar  - hLstar);
    const ComputeType huFlux = ComputeType(0.5) * (fL_hu + fR_hu)
                            - ComputeType(0.5) * alpha * (huRstar - huLstar);

    // Well-balanced bed source term (split form) using reconstructed depths
    const ComputeType psi = -ComputeType(0.5) * G * (hLstar + hRstar) * (bR - bL);

    // Symmetric rounding to storage precision
    const ComputeType hFlux_cast   = static_cast<ComputeType>(hFlux);
    const ComputeType huNetUpdateLeft_cast  = static_cast<ComputeType>( huFlux- ComputeType(0.5) * psi);
    const ComputeType huNetUpdateRight_cast  = static_cast<ComputeType>(-huFlux- ComputeType(0.5) * psi);


    // Net updates (left gets +flux, right gets -flux), add bed split
    hNetUpdateLeft   =  static_cast<RealType>(hFlux_cast);
    huNetUpdateLeft  =  static_cast<ComputeType>(huNetUpdateLeft_cast);
    hNetUpdateRight  = static_cast<RealType>(-hFlux_cast);
    huNetUpdateRight = static_cast<ComputeType>(huNetUpdateRight_cast);

    // CFL edge speed
    maxEdgeSpeed = alpha;

#ifdef DEBUG
    std::cout << "RusanovMixed:\n"
              << "  hL=" << static_cast<float>(hL) << ", huL=" << static_cast<float>(huL) << ", bL=" << static_cast<float>(bL) << "\n"
              << "  hR=" << static_cast<float>(hR) << ", huR=" << static_cast<float>(huR) << ", bR=" << static_cast<float>(bR) << "\n"
              << "  hL*=" << static_cast<float>(hLstar) << ", huL*=" << static_cast<float>(huLstar)
              << ", hR*=" << static_cast<float>(hRstar) << ", huR*=" << static_cast<float>(huRstar) << "\n"
              << "  uL=" << static_cast<float>(uL) << ", uR=" << static_cast<float>(uR)
              << ", cL=" << static_cast<float>(cL) << ", cR=" << static_cast<float>(cR) << ", alpha=" << static_cast<float>(alpha) << "\n"
              << "  fL_Hu=" << static_cast<float>(fL_hu) << ", fR_Hu=" << static_cast<float>(fR_hu) << ", 0.5*(fL_hu+fR_hu)=" << static_cast<float>((ComputeType(0.5) * (fL_hu  + fR_hu ))) << "\n"
              << "  hFlux=" << static_cast<float>(hFlux) << ", huFlux=" << static_cast<float>(huFlux) << ", psi=" << static_cast<float>(psi) << "\n"
              << "  hΔL=" << static_cast<float>(hNetUpdateLeft) << ", hΔR=" << static_cast<float>(hNetUpdateRight) << "\n"
              << "  huΔL=" << static_cast<float>(huNetUpdateLeft) << ", huΔR=" << static_cast<float>(huNetUpdateRight) << "\n"
              << "  maxEdgeSpeed=" << static_cast<float>(maxEdgeSpeed) << "\n\n";
#endif
  }

  void RusanovMixed::applyBoundaryCondition(
    ComputeType& hL, ComputeType& hR,
    ComputeType& huL, ComputeType& huR,
    ComputeType& bL, ComputeType& bR)
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
