/**
* @file HLLC.cpp
* HLLC (Harten-Lax-van Leer-Contact) for shallow water equations
*
* This HLLC solver is implemented using the theories and explanations provided by the following sources:
* Toro, E. F. (1999). Riemann Solvers and Numerical Methods for Fluid Dynamics: A Practical Introduction (2nd ed.). Springer, https://link.springer.com/book/10.1007/b79761 and
* Toro, E. F., Spruce, M., & Speares, W. (1994). Restoration of the contact surface in the HLL-Riemann solver. Shock Waves, 4(1), 25–34., https://link.springer.com/article/10.1007/BF01414629
*
* The exact use of the bathymetry might not be correct and therefore the solver might not be accurate for all scenarios
*/

#include "HLLC.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

#include "Tools/RealMath.hpp"

void Solvers::HLLC::computeNetUpdates(
 const RealType& hLTrueValue,
 const RealType& hRTrueValue,
 const RealType& huLTrueValue,
 const RealType& huRTrueValue,
 const RealType& bLTrueValue,
 const RealType& bRTrueValue,
 RealType&       hNetUpdateLeft,
 RealType&       hNetUpdateRight,
 RealType&       huNetUpdateLeft,
 RealType&       huNetUpdateRight,
 RealType&       maxEdgeSpeed
) {
 // Initialize local variables so that altering h, hu, b for reflective boundary conditions doesn't change original data
 RealType hL = hLTrueValue;
 RealType hR = hRTrueValue;
 RealType huL = huLTrueValue;
 RealType huR = huRTrueValue;
 RealType bL = bLTrueValue;
 RealType bR = bRTrueValue;

 // set h, hu, b if at least one cell is dry
 applyBoundaryCondition(hL, hR, huL, huR, bL, bR);

 double source_term = -G * 0.5 * (hL+hR) * (bR-bL);

 RealType uL = (huL / hL);
 RealType uR = (huR / hR);

 const RealType cL = sqrt_real(G * hL);
 const RealType cR = sqrt_real(G * hR);

 // Linke und rechte Wellengeschw:
 RealType SL = min_real(uL - cL, uR - cR);
 RealType SR = max_real(uL + cL, uR + cR);

 // Falls alles nach rechts (>0) oder alles nach links (<0) läuft,
 // kann man direkt einen reinen linken/rechten Flux nehmen:
 if (SL >= 0.0) {
   // Komplett nach rechts: Flux = fL
   const RealType fL_h  = huL;
   const RealType fL_hu = huL * uL + 0.5 * G * hL * hL;

   // Links verliert, rechts gewinnt => wir verteilen den Flux:
   hNetUpdateLeft  =  fL_h;
   huNetUpdateLeft =  fL_hu;
   hNetUpdateRight  = -fL_h;
   huNetUpdateRight = -fL_hu;

   //da hier konnte ich noch nicht gut testen
   huNetUpdateLeft -= source_term * 0.5;
   huNetUpdateRight -= source_term * 0.5;

   maxEdgeSpeed = SL; // oder SR (hier beide >= 0)
   return;
 }
 if (SR <= 0.0) {
   // Komplett nach links: Flux = fR
   const RealType fR_h  = huR;
   const RealType fR_hu = huR * uR + 0.5 * G * hR * hR;

   // R verliert, l gewinnt:
   hNetUpdateLeft  = -fR_h;
   huNetUpdateLeft = -fR_hu;
   hNetUpdateRight  =  fR_h;
   huNetUpdateRight =  fR_hu;

   //da hier konnte ich noch nicht gut testen
   huNetUpdateLeft -= source_term * 0.5;
   huNetUpdateRight -= source_term * 0.5;

   maxEdgeSpeed = abs_real(SR);
   return;
 }

 // Druck
 RealType pL = 0.5 * G * hL * hL;
 RealType pR = 0.5 * G * hR * hR;

 RealType numerator   = huR * (SR - uR) - huL * (SL - uL) + (pL - pR);
 RealType denominator = hR  * (SR - uR) - hL  * (SL - uL);
 RealType SM = numerator / denominator;  // Mittlere Geschw.

 RealType hL_star  = hL  * ((SL - uL) / (SL - SM));
 RealType huL_star = hL_star * SM;

 RealType hR_star  = hR  * ((SR - uR) / (SR - SM));
 RealType huR_star = hR_star * SM;

 // Flux links/rechts
 RealType fL_h  = huL;
 RealType fL_hu = huL * uL + 0.5 * G * hL * hL;

 RealType fR_h  = huR;
 RealType fR_hu = huR * uR + 0.5 * G * hR * hR;

 RealType hFlux  = 0.0;
 RealType huFlux = 0.0;

 if (SL <= 0.0 && 0.0 <= SM) {
   // linker Sternzustand
   hFlux  = fL_h  + SL * (hL_star  - hL);
   huFlux = fL_hu + SL * (huL_star - huL);
 }
 else if (SM <= 0.0 && 0.0 <= SR) {
   // rechter Sternzustand
   hFlux  = fR_h  + SR * (hR_star  - hR);
   huFlux = fR_hu + SR * (huR_star - huR);
 }
 else if (SM >= 0.0) {
   // 0 liegt links von SM -> alles wie links
   hFlux  = fL_h;
   huFlux = fL_hu;
 }
 else {
   // SM <= 0, 0 rechts von SM -> alles wie rechts
   hFlux  = fR_h;
   huFlux = fR_hu;
 }

 hNetUpdateLeft   = hFlux;
 huNetUpdateLeft  = huFlux;
 hNetUpdateRight  =  -hFlux;
 huNetUpdateRight =  -huFlux;

 huNetUpdateLeft -= source_term * 0.5;
 huNetUpdateRight -= source_term * 0.5;

 maxEdgeSpeed = max_real(abs_real(SL), max_real(abs_real(SR), abs_real(SM)));

#ifdef DEBUG
 std::cout << "HLLC classical solver:\n"
           << "   hL=" << hL << ",  huL=" << huL << ",  uL=" << uL << "\n"
           << "   hR=" << hR << ",  huR=" << huR << ",  uR=" << uR << "\n"
           << "   SL=" << SL << ",  SM=" << SM << ",  SR=" << SR << "\n"
           << "   hL*=" << hL_star << ",  huL*=" << huL_star << "\n"
           << "   hR*=" << hR_star << ",  huR*=" << huR_star << "\n"
           << "   hFlux=" << hFlux << ", huFlux=" << huFlux << "\n"
           << "   hNetUpdateLeft=" << hNetUpdateLeft
           << ",   hNetUpdateRight=" << hNetUpdateRight << "\n"
           << "   huNetUpdateLeft=" << huNetUpdateLeft
           << ",  huNetUpdateRight=" << huNetUpdateRight << "\n"
           << "   maxEdgeSpeed=" << maxEdgeSpeed << "\n\n";
#endif
}

void Solvers::HLLC::applyBoundaryCondition(RealType& hL, RealType& hR, RealType& huL, RealType& huR, RealType& bL, RealType& bR) {
  if (bL >= 0.0) {
    hL = hR;
    huL = -huR;
    bL = bR;
  } else if (bR >= 0.0) {
    hR = hL;
    huR = -huL;
    bR = bL;
  }
}
