/**
* @file WavePropagationBlockBFloat.hpp
*
* original author: Sebastian Rettenberger <rettenbs@in.tum.de>
* modified by: Lena Elisabeth Holtmannspötter
 */

#pragma once

#include "FWaveSolver.hpp"
#include "Solver/HLLC.hpp"
#include "Solver/Osher.hpp"
#include "Tools/RealType.hpp"
//#include "/Users/lenaholtmannspoetter/Desktop/HALF/swe1d/build/_deps/swe-solvers-src/Source/FWaveSolver.hpp"
//#include "/Users/lenaholtmannspoetter/Desktop/HALF/swe1d/build/_deps/swe-solvers-src/Source/HybridWaveSolver.hpp"
#include "Solver/FWaveSolver_Original.hpp"
#include "Solver/FWaveSolver_Mixed.hpp"
#include "Solver/RusanovWetDry.hpp"
#include "Solver/RusanovMixed.hpp"
#include "Solver/RusanovMixedBFloat.hpp"
#include "Solver/HLLCWetDry.hpp"

namespace Blocks {

  /**
   * Allocated variables:
   *   unknowns h,hu are defined on grid indices [0,..,n+1] (done by the caller)
   *     -> computational domain is [1,..,nx]
   *     -> plus ghost cell layer
   *
   *   net-updates are defined for edges with indices [0,..,n]
   *
   * A left/right net update with index (i-1) is located on the edge between
   *   cells with index (i-1) and (i):
   * <pre>
   *   *********************
   *   *         *         *
   *   *  (i-1)  *   (i)   *
   *   *         *         *
   *   *********************
   *
   *             *
   *            ***
   *           *****
   *             *
   *             *
   *    NetUpdatesLeft(i-1)
   *             or
   *    NetUpdatesRight(i-1)
   * </pre>
   */
  class WavePropagationBlockBFloat {
  public:
    enum BoundaryCondition {
      ReflectingBoundary,
      OutflowBoundary
    };
  private:
    ComputeType* h_;
    ComputeType* hu_;
    RealType* b_;

    ComputeType* hNetUpdatesLeft_;
    ComputeType* hNetUpdatesRight_;

    RealType* huNetUpdatesLeft_;
    RealType* huNetUpdatesRight_;

    unsigned int size_;

    ComputeType cellSize_;

    BoundaryCondition leftBoundary_;
    BoundaryCondition rightBoundary_;


    /** The solver used in computeNumericalFluxes */
    //Solvers::HLL solver_with_bathymetry_;
    //Solvers::FWaveSolver_Mixed<RealType, ComputeType> solver_with_bathymetry_;
    //Solvers::FWaveSolver_Original<RealType, ComputeType> solver_with_bathymetry_;
    Solvers::RusanovMixedBFloat solver_with_bathymetry_;

  public:
    /**
     * @param size Domain size (= number of cells) without ghost cells
     * @param cellSize Size of one cell
     */
    WavePropagationBlockBFloat(ComputeType* h, ComputeType* hu, RealType* b, unsigned int size, ComputeType cellSize);
    ~WavePropagationBlockBFloat();

    /**
     * Computes the net-updates from the unknowns
     *
     * @return The maximum possible time step
     */
    ComputeType computeNumericalFluxes();

    /**
     * Update the unknowns with the already computed net-updates
     *
     * @param dt Time step size
     */
    void updateUnknowns(ComputeType dt);

    /**
     * Updates h, hu and b according to the set condition on both
     * boundaries
     */
    void applyBoundaryConditions();

    /**
     * Sets left boundary condition to parameter
     *
     * Do NOT call when simulation is running, will result in unexpected behaviour
     * @param condition boundary condition, that should be implemented on the left border
     */
    void setLeftBoundaryCondition(BoundaryCondition condition);

    /**
     * Sets right boundary condition to parameter
     *
     * Do NOT call when simulation is running, will result in unexpected behaviour
     * @param condition boundary condition, that should be implemented on the right border
     */
    void setRightBoundaryCondition(BoundaryCondition condition);
  };

} // namespace Blocks