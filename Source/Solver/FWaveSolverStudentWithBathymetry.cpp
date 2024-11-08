/**
 * @file FWaveSolverStudentWithBathymetry.cpp
 */

#include <cassert>
#include <cmath>
#include <iostream>

#include "FWaveSolverStudentWithBathymetry.hpp"


void Solvers::FWaveSolverStudentWithBathymetry::computeNetUpdates(
const RealType& hLTrueValue, const RealType& hRTrueValue,
const RealType& huLTrueValue, const RealType& huRTrueValue,
const RealType& bLTrueValue, const RealType& bRTrueValue,
RealType& hNetUpdateLeft,
RealType& hNetUpdateRight,
RealType& huNetUpdateLeft,
RealType& huNetUpdateRight,
RealType& maxEdgeSpeed)
{
  // Initialize local variables so that altering h, hu, b for reflective boundary conditions doesn't change original data
  RealType hL = hLTrueValue;
  RealType hR = hRTrueValue;
  RealType huL = huLTrueValue;
  RealType huR = huRTrueValue;
  RealType bL = bLTrueValue;
  RealType bR = bRTrueValue;

  // set h, hu, b if at least one cell is dry
  applyBoundaryCondition(hL, hR, huL, huR, bL, bR);

  // Don't do anything if both cells are dry
  if (bL >= 0.0 && bR >= 0.0) {
    hNetUpdateLeft = hNetUpdateRight = huNetUpdateLeft = huNetUpdateRight = 0.0;
    return;
  }

  // Compute velocity for left and right sides
  assert(hL > 0.0 && "hL must be greater than zero to avoid division by zero.");
  assert(hR > 0.0 && "hR must be greater than zero to avoid division by zero.");
  RealType uL = huL/hL;
  RealType uR = huR/hR;

  // Compute Roe eigenvalues
  RealType eigenvalues[2] = { 0.0, 0.0 };
  computeEigenvalues(hL, hR, huL, huR, eigenvalues);

  // Compute flux differences (delta F)
  RealType fluxDif[2] = { 0.0, 0.0 };
  computeFluxDifferences(hL, hR, huL, huR, uL, uR, bL, bR, fluxDif);
  
  // this assert should never fail, as hL > 0 and hR > 0 is already asserted above
  assert(eigenvalues[0] != eigenvalues[1] && "The values of the Roe eigenvalues must be different to avoid division by zero.");

  // Compute alphas
  RealType alphas[2] = { 0.0, 0.0 };
  computeAlphas(alphas, fluxDif, eigenvalues);

  // reset hNetUpdates and huNetUpdates
  hNetUpdateLeft = hNetUpdateRight = huNetUpdateLeft = huNetUpdateRight = 0.0;
  /// Compute net updates for height and momentum for first wave
  calculateNetUpdates(hNetUpdateLeft, hNetUpdateRight, huNetUpdateLeft, huNetUpdateRight, alphas, eigenvalues);

  RealType waveSpeedLeft = 0;
  RealType waveSpeedRight = 0;
  calculateWaveSpeed(waveSpeedLeft, waveSpeedRight, eigenvalues);
  // Set wave speeds according to signs of eigenvalues
  
  // Compute the maximum speed
  maxEdgeSpeed = std::fmax(std::abs(eigenvalues[0]), std::abs(eigenvalues[1]));

  // x(hL, hR, huL, huR, bL, bR, hNetUpdateLeft, hNetUpdateRight, huNetUpdateLeft, huNetUpdateRight, maxEdgeSpeed);
}

void Solvers::FWaveSolverStudentWithBathymetry::applyBoundaryCondition(RealType& hL, RealType& hR, RealType& huL, RealType& huR, RealType& bL, RealType& bR) {
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

void Solvers::FWaveSolverStudentWithBathymetry::computeEigenvalues(RealType hL, RealType hR, RealType huL, RealType huR, RealType eigenvalues[2]) {
  // Compute hRoe and uRoe

  RealType hRoe = 0.5 * (hL + hR);
  RealType uRoe = (huL / std::sqrt(hL) + huR/std::sqrt(hR)) / (std::sqrt(hL) + std::sqrt(hR));

  // Compute Roe eigenvalues
  RealType sqrtGHRoe = std::sqrt(G * hRoe);
  eigenvalues[0] = uRoe - sqrtGHRoe;
  eigenvalues[1] = uRoe + sqrtGHRoe;
}

void Solvers::FWaveSolverStudentWithBathymetry::computeFluxDifferences(RealType hL, RealType hR, RealType huL, RealType huR, RealType uL, RealType uR, RealType bL, RealType bR, RealType fluxDif[2]) {
  RealType G = 9.81;
  // Compute Psi for lux difference
  RealType psi = -0.5*G*(bR - bL)*(hL + hR);
  fluxDif[0] = huR - huL;
  fluxDif[1] = huR*uR + 0.5*G*hR*hR - huL*uL - 0.5*G*hL*hL;
  fluxDif[1] -= psi;
}

void Solvers::FWaveSolverStudentWithBathymetry::computeAlphas(RealType alphas[2], RealType fluxDif[2], RealType eigenvalues[2]) {
  RealType inverseFactor = 1/(eigenvalues[1] - eigenvalues[0]);
  alphas[0] = eigenvalues[1]*fluxDif[0] - fluxDif[1];
  alphas[0] *= inverseFactor;
  alphas[1] = -eigenvalues[0]*fluxDif[0] + fluxDif[1];
  alphas[1] *= inverseFactor;
}

void Solvers::FWaveSolverStudentWithBathymetry::calculateNetUpdates(RealType& hNetUpdateLeft, RealType& hNetUpdateRight, RealType& huNetUpdateLeft, RealType& huNetUpdateRight,RealType alphas[2], RealType eigenvalues[2]) {
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
}

void Solvers::FWaveSolverStudentWithBathymetry::calculateWaveSpeed(RealType& waveSpeedLeft, RealType& waveSpeedRight, RealType eigenvalues[2]) {
  waveSpeedLeft = eigenvalues[0];
  waveSpeedRight = eigenvalues[1];
  if (eigenvalues[0] < 0 && eigenvalues[1] < 0) {
    waveSpeedRight = 0.0;
  } else if (eigenvalues[0] > 0 && eigenvalues[1] > 0) {
    waveSpeedLeft = 0.0;
  }
}