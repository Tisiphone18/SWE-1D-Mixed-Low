/**
* @file Osher.cpp
 *
 *  This Osher (more specific Dumbser-Osher-Toro) solver is implemented using the theories and explanations provided in the following sources:
 *  Dumbser, M., Toro, E.F. A Simple Extension of the Osher Riemann Solver to Non-conservative Hyperbolic Systems. J Sci Comput 48, 70–88 (2011). https://doi.org/10.1007/s10915-010-9400-3
 *  Castro, M.J., Gallardo, J.M., Marquina, A. (2016). Approximate Osher-Solomon Schemes for Hyperbolic Systems. In: Ortegón Gallego, F., Redondo Neble, M., Rodríguez Galván, J. (eds) Trends in Differential Equations and Applications. SEMA SIMAI Springer Series, vol 8. Springer, Cham. https://doi.org/10.1007/978-3-319-32013-7_1
 *
 *  Furthermore, the following book was helpful:
 *  Toro, Eleuterio. (2009). Riemann Solvers and Numerical Methods for Fluid Dynamics: A Practical Introduction. https://doi.org/10.1007/b79761.
 */

#include "Osher.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

void Solvers::OsherSolver::computeNetUpdates(
  const RealType& hLTrueValue, const RealType& hRTrueValue,
  const RealType& huLTrueValue, const RealType& huRTrueValue,
  [[maybe_unused]] const RealType& bLTrueValue, [[maybe_unused]] const RealType& bRTrueValue,
  RealType& hNetUpdateLeft,
  RealType& hNetUpdateRight,
  RealType& huNetUpdateLeft,
  RealType& huNetUpdateRight,
  RealType& maxEdgeSpeed)
{
  // Local copies
  RealType hL = hLTrueValue, hR = hRTrueValue;
  RealType huL = huLTrueValue, huR = huRTrueValue;
  RealType bL = bLTrueValue,  bR = bRTrueValue; // kept only to satisfy signature

  // Optional: basic dry handling (no bathymetry logic)
  applyBoundaryCondition(hL, hR, huL, huR, bL, bR);

  // Guard tiny/negative
  const RealType hLpos = max_real(hL, H_MIN);
  const RealType hRpos = max_real(hR, H_MIN);

  // Speeds from raw states (bathymetry-free)
  const RealType uL = (hL > DRY_TOL) ? (huL / hLpos) : RealType(0);
  const RealType uR = (hR > DRY_TOL) ? (huR / hRpos) : RealType(0);

  // Osher integral of |A| along straight segment
  RealType integralResult[2][2] = {{0,0},{0,0}};
  maxEdgeSpeed = 0;

  for (int i = 0; i < 3; ++i) {
    RealType pathQ[2];
    computeSegmentPath(hL, hR, huL, huR, bL, bR, points[i], pathQ);

    RealType eigenvalues[2] = {0,0};
    computeEigenvalues(pathQ[0], pathQ[1], eigenvalues);

    RealType Aabs[2][2] = {{0,0},{0,0}};
    computeAbsoluteJacobian(eigenvalues, Aabs);

    integralResult[0][0] += Aabs[0][0] * weights[i];
    integralResult[0][1] += Aabs[0][1] * weights[i];
    integralResult[1][0] += Aabs[1][0] * weights[i];
    integralResult[1][1] += Aabs[1][1] * weights[i];

    maxEdgeSpeed = max_real(maxEdgeSpeed,
                            max_real(abs_real(eigenvalues[0]), abs_real(eigenvalues[1])));
  }

  // Difference and arithmetic flux (no gravity/bathymetry correction)
  const RealType deltaQ0 = RealType(0.5) * (hR - hL);
  const RealType deltaQ1 = RealType(0.5) * (huR - huL);

  RealType fluxFunction0 = RealType(0.5) * (huR + huL);
  RealType fluxFunction1 = RealType(0.5) * (huL * uL + huR * uR
                                            + RealType(0.5) * G * (hL * hL + hR * hR));

  RealType flux0 = fluxFunction0 - (integralResult[0][0] * deltaQ0 + integralResult[0][1] * deltaQ1);
  RealType flux1 = fluxFunction1 - (integralResult[1][0] * deltaQ0 + integralResult[1][1] * deltaQ1);

  // Return as net updates
  hNetUpdateLeft   =  flux0;
  huNetUpdateLeft  =  flux1;
  hNetUpdateRight  = -flux0;
  huNetUpdateRight = -flux1;
}

void Solvers::OsherSolver::computeSegmentPath(RealType hL, RealType hR, RealType huL, RealType huR,
                                              [[maybe_unused]] RealType bL, [[maybe_unused]] RealType bR,
                                              const RealType s, RealType resultQ[2]) {
  resultQ[0] = hL  + s * (hR  - hL);
  resultQ[1] = huL + s * (huR - huL);
}

void Solvers::OsherSolver::computeEigenvalues(RealType h, RealType hu, RealType eigenvalues[2]) {
  const RealType hpos = max_real(h, H_MIN);
  const RealType u = (h > DRY_TOL) ? (hu / hpos) : RealType(0);
  const RealType c = sqrt_real(G * hpos);
  eigenvalues[0] = u + c;
  eigenvalues[1] = u - c;
}

void Solvers::OsherSolver::computeAbsoluteJacobian(RealType eigenvalues[2], RealType Aabs[2][2]) {
  const RealType a  = eigenvalues[0], b = eigenvalues[1];
  const RealType aa = abs_real(a),    bb = abs_real(b);
  const RealType d  = a - b;
  const RealType scale = max_real(max_real(RealType(1), aa), bb);

  if (abs_real(d) <= EPS_LAM * scale) {
    const RealType m = RealType(0.5) * (aa + bb);
    Aabs[0][0] = m; Aabs[0][1] = 0;
    Aabs[1][0] = 0; Aabs[1][1] = m;
    return;
  }
  Aabs[0][0] = (-b * aa + a * bb) / d;
  Aabs[0][1] = (aa - bb) / d;
  Aabs[1][0] = (a * b * (bb - aa)) / d;
  Aabs[1][1] = (a * aa - b * bb) / d;
}

// Now a no-bathymetry, minimal boundary/dry handler
void Solvers::OsherSolver::applyBoundaryCondition(RealType& hL, RealType& hR,
                                                  RealType& huL, RealType& huR,
                                                  [[maybe_unused]] RealType& bL,
                                                  [[maybe_unused]] RealType& bR)
{
  // Zero tiny depths/momenta; no use of bathymetry at all
  if (hL < DRY_TOL) { hL = 0; huL = 0; }
  if (hR < DRY_TOL) { hR = 0; huR = 0; }

  // Optional: reflective at wet–dry interface (purely algebraic)
  if (hL < DRY_TOL && hR >= DRY_TOL) {
    hL  = hR;
    huL = -huR;
  } else if (hR < DRY_TOL && hL >= DRY_TOL) {
    hR  = hL;
    huR = -huL;
  }
}
