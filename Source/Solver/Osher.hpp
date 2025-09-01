/**
* @file Osher.hpp
*
*  Osher-type Solver for shallow water equations
**
*  This Osher (more specific Dumbser-Osher-Toro) solver is implemented using the theories and explanations provided by the following sources:
*  Dumbser, M., Toro, E.F. A Simple Extension of the Osher Riemann Solver to Non-conservative Hyperbolic Systems. J Sci Comput 48, 70–88 (2011). https://doi.org/10.1007/s10915-010-9400-3
*  Castro, M.J., Gallardo, J.M., Marquina, A. (2016). Approximate Osher-Solomon Schemes for Hyperbolic Systems. In: Ortegón Gallego, F., Redondo Neble, M., Rodríguez Galván, J. (eds) Trends in Differential Equations and Applications. SEMA SIMAI Springer Series, vol 8. Springer, Cham. https://doi.org/10.1007/978-3-319-32013-7_1
*
*  Furthermore, the following book was helpful:
*  Toro, Eleuterio. (2009). Riemann Solvers and Numerical Methods for Fluid Dynamics: A Practical Introduction. https://doi.org/10.1007/b79761.
*  Bachelorpraktikum - Lena Holtmannspötter, Jonathan Pins, Johannes Karrer
 */

#pragma once

#include <cmath>

#include "Tools/RealType.hpp"
#include "Tools/RealMath.hpp"

namespace Solvers {
  class OsherSolver{
  public:
    // Physical constant: gravity
    const RealType G = 9.81; // in (m/s^2)
    const RealType weights[3] = { RealType(5) / RealType(18), RealType(8) / RealType(18), RealType(5) / RealType(18) };
    const RealType points[3] = { RealType(0.5) - sqrt_real(RealType(15)) / RealType(10), RealType(0.5), RealType(0.5) + sqrt_real(RealType(15)) / RealType(10) };


    // ------------------------------
    // Precision-dependent tolerances
    // Picked once at compile time.
    // ------------------------------
#if defined(ENABLE_SINGLE_PRECISION)
    // ===== float (FP32) =====
    static inline const RealType H_MIN   = RealType(3.5e-4);
    static inline const RealType DRY_TOL = RealType(3.5e-3);
    static inline const RealType EPS_LAM = RealType(1.2e-1);

#elif defined(ENABLE_HALF_PRECISION)
    // ===== half (FP16) =====
    static inline const RealType H_MIN   = RealType(3.1e-2);
    static inline const RealType DRY_TOL = RealType(3.1e-1);
    static inline const RealType EPS_LAM = RealType(1.1);

#elif defined(ENABLE_BF16_PRECISION)
    // ===== bfloat16 (BF16) =====
    static inline const RealType H_MIN   = RealType(8.8e-2);
    static inline const RealType DRY_TOL = RealType(8.8e-1);
    static inline const RealType EPS_LAM = RealType(1.9);

#else
    // ===== default: double (FP64) =====
    static inline const RealType H_MIN   = RealType(1.5e-8);
    static inline const RealType DRY_TOL = RealType(1.5e-7);
    static inline const RealType EPS_LAM = RealType(7.7e-4);
#endif



    void computeNetUpdates(
      const RealType& hLTrueValue, const RealType& hRTrueValue,
      const RealType& huLTrueValue, const RealType& huRTrueValue,
      const RealType& bLTrueValue, const RealType& bRTrueValue,
      RealType& hNetUpdateLeft,
      RealType& hNetUpdateRight,
      RealType& huNetUpdateLeft,
      RealType& huNetUpdateRight,
      RealType& maxEdgeSpeed);

    /**
    *
    * @param h parameter height to evaluate value of eigenvalues with
    * @param hu parameter momentum to evaluate value of eigenvalues with
    * @param eigenvalues destination of computed eigenvalues
     */
    void computeEigenvalues(RealType h, RealType hu, RealType eigenvalues[2]);

    /**
    * Computes the value of the simple segment path between left and right state at point s
    *
    * @param hL height on left side
    * @param hR height on right side
    * @param huL momentum on left side
    * @param huR momentum on right side
    * @param bL bathymetry on left side (unused in this version of the solver)
    * @param bR bathymetry on right side (unused in this version of the solver)
    * @param s point where the segment path will be evaluated at
    * @param resultQ state according to the chosen simple segment path at point s
     */
    void computeSegmentPath(RealType hL, RealType hR, RealType huL, RealType huR, RealType bL, RealType bR, RealType s, RealType resultQ[2]);

    /**
    * Computes the absolute Jacobian
    *
    * @param eigenvalues eigenvalues
    * @param absoluteJacobian computed absolute Jacobian matrix will be stored here
     */
    void computeAbsoluteJacobian(RealType eigenvalues[2], RealType absoluteJacobian[2][2]);

    void applyBoundaryCondition(RealType& hL, RealType& hR, RealType& huL, RealType& huR, RealType& bL, RealType& bR) ;
  };
}
