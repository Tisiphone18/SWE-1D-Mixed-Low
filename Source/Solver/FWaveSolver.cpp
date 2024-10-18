#include "FWaveSolver.h"

#include <cmath>

//TODO double <-> Real Type
//TODO const in header
//TODO documentation
//TODO sqrt

/**
 * @brief Computes the net updates for the shallow water equations at an edge between two cells.
 *
 * ... ...
 *
 * @param hL Water height on the left side of the edge.
 * @param hR Water height on the right side of the edge.
 * @param huL Water momentum on the left side of the edge.
 * @param huR Water momentum on the right side of the edge.
 * @param bL Bathymetry value on the left side of the edge.
 * @param bR Bathymetry value on the right side of the edge.
 * @param[out] hNetUpdateLeft Net update for water height to the left cell.
 * @param[out] hNetUpdateRight Net update for water height to the right cell.
 * @param[out] huNetUpdateLeft Net update for momentum to the left cell.
 * @param[out] huNetUpdateRight Net update for momentum to the right cell.
 * @param[out] maxEdgeSpeed The maximum wave speed across the edge.
 */

void FWaveSolver::computeNetUpdates(
  double hL, double hR,
  double huL, double huR,
  double bL, double bR,
  double& hNetUpdateLeft,
  double& hNetUpdateRight,
  double& huNetUpdateLeft,
  double& huNetUpdateRight,
  double& maxEdgeSpeed)
{

  // Physical constant
  const double g = 9.81; // gravity

  // Compute hRoe and uRoe
  double hRoe = 0.5 * (hL + hR);
  double uRoe = (huL * sqrt(hL) + huR * sqrt(hR)) / (sqrt(hL) + sqrt(hR));

  // Compute Roe eigenvalues
  double sqrt_g_hRoe = sqrt(9.81 * hRoe);
  double lambda1 = uRoe - sqrt_g_hRoe;
  double lambda2 = uRoe + sqrt_g_hRoe;

  // Determine the maximum edge speed
  maxEdgeSpeed = std::fmax(std::abs(lambda1), std::abs(lambda2));

  // Decomposition of the jump in flux function
  double delta_h = hR - hL;
  double delta_hu = huR - huL;

  // Calculate alpha
  double alpha1 = 0.5 * (delta_h - delta_hu / sqrt_g_hRoe);
  double alpha2 = 0.5 * (delta_h + delta_hu / sqrt_g_hRoe);

  // Compute the wave updates
  hNetUpdateLeft = alpha1 * lambda1 < 0 ? lambda1 * alpha1 : 0;
  hNetUpdateRight = alpha2 * lambda2 > 0 ? lambda2 * alpha2 : 0;
  huNetUpdateLeft = alpha1 * lambda1 < 0 ? lambda1 * alpha1 : 0;
  huNetUpdateRight = alpha2 * lambda2 > 0 ? lambda2 * alpha2 : 0;
}
