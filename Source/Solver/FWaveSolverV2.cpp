#include "FWaveSolverV2.hpp"
#include <cmath>
#include <iostream>
#include <cassert>


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

  // Physical constant: gravity
  const RealType g = 9.81; // in (m/s^2)

  // Compute velocity for left and right sides
  //TODO : assert h > 0 ?
  //assert(hL > 0 && "hL must be greater than zero to avoid division by zero.");
  //assert(hR > 0 && "hR must be greater than zero to avoid division by zero.");
  RealType uL = huL/hL;
  RealType uR = huR/hR;

  // Compute hRoe and uRoe
  RealType hRoe = 0.5 * (hR + hL);
  RealType uRoe = uL*std::sqrt(hL)+uR*std::sqrt(hR);
  uRoe /= std::sqrt(hL)+std::sqrt(hR);

  // Compute Roe eigenvalues
  RealType x = std::sqrt(g*hRoe);
  RealType lambda1 = uRoe - x;
  RealType lambda2 = uRoe + x;

  // Compute flux differences (delta F)
  RealType halfG = 0.5*g;
  RealType deltaF[2] = {huR - huL, hR*uR*uR+halfG*hR*hR-hL*uL*uL-halfG*hL*hL};
  //TODO : assert lambda1 != lambda2
  //assert(lambda1 != lambda2 && "The values of the Roe eigenvalues must be different to avoid division by zero.");
  RealType inverse_mul = 1/(lambda2 - lambda1);

  RealType alpha1 = lambda2*deltaF[0]-deltaF[1];
  alpha1 *= inverse_mul;
  RealType alpha2 = -lambda1*deltaF[0]+deltaF[1];
  alpha2 *= inverse_mul;

  // Compute net updates for height and momentum on the left side
  // TODO : können wir vlt vereinfachen zu :
  //  checke ob VZ unterschiedlich von lambda 1 und lambda 2
  //  -> falls ja so :
  // if (lambda1 < 0) {
  //    hNetUpdateLeft += alpha1;
  //    huNetUpdateLeft += lambda1 * alpha1;
  //}
  // ...
  // TODO : falls VZ gleich ????? -> Wie weiter ?

  hNetUpdateLeft = lambda1 < 0?alpha1:0;
  hNetUpdateLeft += lambda2 < 0?alpha2:0;
  huNetUpdateLeft = lambda1 < 0?lambda1*alpha1:0;
  huNetUpdateLeft += lambda2 < 0?lambda2*alpha2:0;

  // Compute net updates for height and momentum on the right side
  hNetUpdateRight = lambda1 > 0?alpha1:0;
  // std::cout << "lambda" << lambda1 << ":" << lambda2 << ":" << alpha1 << std::endl;
  hNetUpdateRight += lambda2 > 0?alpha2:0;
  huNetUpdateRight = lambda1 > 0?lambda1*alpha1:0;
  huNetUpdateRight += lambda2 > 0?lambda2*alpha2:0;

  // Compute the maximum speed
  maxEdgeSpeed = std::fmax(std::abs(lambda1), std::abs(lambda2));

  //TODO : CFL condition ? Stabilität im 1D : C = (u * deltaT) / (deltaX) muss kleiner als 1 sein (deltaT ≙ Zeitschritt, deltaX ≙ Ortsschritt)
  // => u * deltaT / deltaX < CFL => u * deltaT < CFL * deltaX => deltaT < (CFL * deltaX) / u mit u ≙ max(lambda1, lambda2)
  // => braucht zwei zusätliche Params !
  // CFL Zahl <= 1, save value 0.5
  // assert(deltaT < (CFL * deltaX / lambdaMax) && "The time step size or the mesh step size violate the CFL condition, causing potential instability.");
  //vgl. https://en.wikipedia.org/wiki/Courant–Friedrichs–Lewy_condition
  //vgl. https://www.simscale.com/blog/cfl-condition/
  //TODO : gibts noch mehr asserts ?
}
