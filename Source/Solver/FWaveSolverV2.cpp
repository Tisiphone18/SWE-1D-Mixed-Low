#include "FWaveSolverV2.hpp"

#include <cmath>
#include <iostream>

void Solvers::FWaveSolverStudentV2::computeNetUpdates(
const RealType& hL, const RealType& hR,
const RealType& huL, const RealType& huR,
const RealType& bL, const RealType& bR,
RealType& hNetUpdateLeft,
RealType& hNetUpdateRight,
RealType& huNetUpdateLeft,
RealType& huNetUpdateRight,
RealType& maxEdgeSpeed)
{

  // Physical constant
  const RealType g = 9.81; // gravity

  RealType uL = huL/hL;
  RealType uR = huR/hR;

  // Compute hRoe and uRoe
  RealType hRoe = 0.5 * (hR + hL);
  RealType uRoe = uL*std::sqrt(hL)+uR*std::sqrt(hR);
  uRoe /= std::sqrt(hL)+std::sqrt(hR);

  RealType x = std::sqrt(g*hRoe);
  RealType lambda1 = uRoe - x;
  RealType lambda2 = uRoe + x;

  RealType halfG = 0.5*g;
  RealType deltaF[2] = {huR - huL, hR*uR*uR+halfG*hR*hR-hL*uL*uL-halfG*hL*hL};
  RealType inverse_mul = 1/(lambda2 - lambda1);

  RealType alpha1 = lambda2*deltaF[0]-deltaF[1];
  alpha1 *= inverse_mul;
  RealType alpha2 = -lambda1*deltaF[0]+deltaF[1];
  alpha2 *= inverse_mul;

  hNetUpdateLeft = lambda1 < 0?alpha1:0;
  hNetUpdateLeft += lambda2 < 0?lambda2:0;
  huNetUpdateLeft = lambda1 < 0?lambda1*alpha1:0;
  huNetUpdateLeft += lambda2 < 0?lambda2*alpha2:0;

  hNetUpdateRight = lambda1 > 0?alpha1:0;
  // std::cout << "lambda" << lambda1 << ":" << lambda2 << ":" << alpha1 << std::endl;
  hNetUpdateRight += lambda2 > 0?lambda2:0;
  huNetUpdateRight = lambda1 > 0?lambda1*alpha1:0;
  huNetUpdateRight += lambda2 > 0?lambda2*alpha2:0;
  maxEdgeSpeed = std::fmax(std::abs(lambda1), std::abs(lambda2));
}
