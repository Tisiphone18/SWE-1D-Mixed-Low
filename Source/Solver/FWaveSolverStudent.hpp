/**
 * @headerfile FWaveSolverStudent.hpp
 */

#pragma once

#include "Tools/RealType.hpp"

namespace Solvers {
  class FWaveSolverStudent {
  public:
    // Physical constant: gravity
    const RealType G = 9.81; // in (m/s^2)

  /**
   * @brief Implementation of the F-Wave Solver.
   * This function calculates the net updates for the height and momentum on the left and right sides of the edge using the F-Wave approach.
   * Does not deal with bathymetry.
   * Calls help functions to compute the eignevalues, flux differences, alphas, netUpdates and waveSpeed.
   * 
   * @param[in] hL Water height on the left side of the edge.
   * @param[in] hR Water height on the right side of the edge.
   * @param[in] huL Water momentum on the left side of the edge.
   * @param[in] huR Water momentum on the right side of the edge.
   * @param[in] bL Bathymetry value on the left side of the edge.
   * @param[in] bR Bathymetry value on the right side of the edge.
   * @param[out] hNetUpdateLeft Net update for water height to the left cell.
   * @param[out] hNetUpdateRight Net update for water height to the right cell.
   * @param[out] huNetUpdateLeft Net update for momentum to the left cell.
   * @param[out] huNetUpdateRight Net update for momentum to the right cell.
   * @param[out] maxEdgeSpeed The maximum wave speed across the edge.
   */
    void computeNetUpdates(
      const RealType& hL, const RealType& hR,
      const RealType& huL, const RealType& huR,
      const RealType& bL, const RealType& bR,
      RealType& hNetUpdateLeft,
      RealType& hNetUpdateRight,
      RealType& huNetUpdateLeft,
      RealType& huNetUpdateRight,
      RealType& maxEdgeSpeed);

    /**
     * Computation of the Roe eigenvalues
     *
     * @param hL Water height on left side of edge
     * @param hR Water height on right side of edge
     * @param huL Water momentum on left side of edge
     * @param huR Water momentum on right side of edge
     * @param eigenvalues will be set to: eigenvalues/wave speeds.
     */
    void computeEigenvalues(RealType hL, RealType hR, RealType huL, RealType huR, RealType eigenvalues[2]);
    
    /**
     * @brief Computation of the wave speed. Dealing also with supersonic problems
     * 
     * @param waveSpeedLeft 
     * @param waveSpeedRight 
     * @param eigenvalues Array of eigenvalues.
     */
    void calculateWaveSpeed(RealType& waveSpeedLeft, RealType& waveSpeedRight, RealType eigenvalues[2]);

    /**
     * @brief Computation of the Net Updates for the left and right Height and left and right Momentum
     * 
     * @param hNetUpdateLeft 
     * @param hNetUpdateRight 
     * @param huNetUpdateLeft 
     * @param huNetUpdateRight 
     * @param alphas Array of values for both alphas.
     * @param eigenvalues Array of eigenvalues.
     */
    void calculateNetUpdates(RealType& hNetUpdateLeft, RealType& hNetUpdateRight, RealType& huNetUpdateLeft, RealType& huNetUpdateRight,RealType alphas[2], RealType eigenvalues[2]);
    
    /**
     * @brief Computation of the Alphas. Later used for computing the net Updates.
     * 
     * @param alphas Array of values for both alphas.
     * @param fluxDif Array of deltas of the flux function.
     * @param eigenvalues Array of eigenvalues.
     */
    void computeAlphas(RealType alphas[2], RealType fluxDif[2], RealType eigenvalues[2]);

    /**
     * @brief Computation of the Delta of the Flux function
     * 
     * @param hL Water height on left side of edge
     * @param hR Water height on right side of edge
     * @param huL Water momentum on left side of edge
     * @param huR Water momentum on right side of edge
     * @param uL Velocity on left side of edge
     * @param uR Velocity on right side of edge
     * @param fluxDif Array of deltas of the flux function.
     */
    void computeFluxDifferences(RealType hL, RealType hR, RealType huL, RealType huR, RealType uL, RealType uR, RealType fluxDif[2]);
  };
}
