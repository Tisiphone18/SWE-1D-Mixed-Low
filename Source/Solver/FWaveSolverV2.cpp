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
  // Compute velocity for left and right sides
  assert(hL > 0 && "hL must be greater than zero to avoid division by zero.");
  assert(hR > 0 && "hR must be greater than zero to avoid division by zero.");
  RealType uL = huL/hL;
  RealType uR = huR/hR;

  // Compute Roe eigenvalues
  RealType eigenvalues[2] = { 0.0, 0.0 };
  computeEigenvalues(hL, hR, huL, huR, eigenvalues);

  // Compute flux differences (delta F)
  RealType fluxDif[2] = { 0.0, 0.0 };
  fluxDif[0] = huR - huL;
  fluxDif[1] = huR*uR + 0.5*G*hR*hR - huL*uL - 0.5*G*hL*hL;

  // this assert should never fail, as hL > 0 and hR > 0 is already asserted above
  assert(eigenvalues[0] != eigenvalues[1] && "The values of the Roe eigenvalues must be different to avoid division by zero.");

  // Compute alphas
  RealType alphas[2] = { 0.0, 0.0 };
  RealType inverseFactor = 1/(eigenvalues[1] - eigenvalues[0]);
  alphas[0] = eigenvalues[1]*fluxDif[0] - fluxDif[1];
  alphas[0] *= inverseFactor;
  alphas[1] = -eigenvalues[0]*fluxDif[0] + fluxDif[1];
  alphas[1] *= inverseFactor;

  // reset hNetUpdates and huNetUpdates
  hNetUpdateLeft = hNetUpdateRight = huNetUpdateLeft = huNetUpdateRight = 0.0;
  // Compute net updates for height and momentum for first wave
  if (eigenvalues[0] < 0) {
    hNetUpdateLeft += alphas[0];
    huNetUpdateLeft += alphas[0]*eigenvalues[0];
  } else if (eigenvalues[0] > 0) {
    hNetUpdateRight += alphas[0];
    huNetUpdateRight += alphas[0]*eigenvalues[0];
  }

  // Compute net updates for height and momentum for second wave
  if (eigenvalues[1] < 0) {
    hNetUpdateLeft += alphas[1];
    huNetUpdateLeft += alphas[1]*eigenvalues[1];
  } else if (eigenvalues[1] > 0) {
    hNetUpdateRight += alphas[1];
    huNetUpdateRight += alphas[1]*eigenvalues[1];
  }

  // Set wave speeds according to signs of eigenvalues
  RealType waveSpeedLeft = 0.0;
  RealType waveSpeedRight = 0.0;
  if (eigenvalues[0] < 0 && eigenvalues[1] < 0) {
    waveSpeedLeft = eigenvalues[0];
  } else if (eigenvalues[0] > 0 && eigenvalues[1] > 0) {
    waveSpeedRight = eigenvalues[1];
  }

  // Compute the maximum speed
  maxEdgeSpeed = std::fmax(std::abs(eigenvalues[0]), std::abs(eigenvalues[1]));

  // x(hL, hR, huL, huR, bL, bR, hNetUpdateLeft, hNetUpdateRight, huNetUpdateLeft, huNetUpdateRight, maxEdgeSpeed);
}

void Solvers::FWaveSolverStudentV2::computeEigenvalues(RealType hL, RealType hR, RealType huL, RealType huR, RealType eigenvalues[2]) {
  // Compute hRoe and uRoe

  RealType hRoe = 0.5 * (hL + hR);
  RealType uRoe = (huL / std::sqrt(hL) + huR/std::sqrt(hR)) / (std::sqrt(hL) + std::sqrt(hR));

  // Compute Roe eigenvalues
  RealType sqrtGHRoe = std::sqrt(G * hRoe);
  eigenvalues[0] = uRoe - sqrtGHRoe;
  eigenvalues[1] = uRoe + sqrtGHRoe;
}
