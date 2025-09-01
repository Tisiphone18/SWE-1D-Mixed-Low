/**
* @file HLLC.hpp
* HLLC (Harten-Lax-van Leer-Contact) for shallow water equations
*
* This HLLC solver is implemented using the theories and explanations provided by the following sources:
* Toro, E. F. (1999). Riemann Solvers and Numerical Methods for Fluid Dynamics: A Practical Introduction (2nd ed.). Springer, https://link.springer.com/book/10.1007/b79761 and
* Toro, E. F., Spruce, M., & Speares, W. (1994). Restoration of the contact surface in the HLL-Riemann solver. Shock Waves, 4(1), 25–34., https://link.springer.com/article/10.1007/BF01414629
*
* The exact use of the bathymetry might not be correct and therefore the solver might not be accurate for all scenarios
*/

#pragma once

#include "Tools/RealType.hpp"

namespace Solvers {
 class HLLC{
 public:

      // Physical constant: gravity
      const RealType G = 9.81; // in (m/s^2)
   /**
* @brief Implementation of the HLLC Solver.
*
* This function calculates the net updates for the height and momentum
* on the left and right sides of the edge using the HLLC approach.
*
* @param[in] hLTrueValue Water height on the left side of the edge.
* @param[in] hRTrueValue Water height on the right side of the edge.
* @param[in] huLTrueValue Water momentum on the left side of the edge.
* @param[in] huRTrueValue Water momentum on the right side of the edge.
* @param[in] bLTrueValue Bathymetry value on the left side of the edge.
* @param[in] bRTrueValue Bathymetry value on the right side of the edge.
* @param[out] hNetUpdateLeft Net update for water height to the left cell.
* @param[out] hNetUpdateRight Net update for water height to the right cell.
* @param[out] huNetUpdateLeft Net update for momentum to the left cell.
* @param[out] huNetUpdateRight Net update for momentum to the right cell.
* @param[out] maxEdgeSpeed The maximum wave speed across the edge.
    */

   void computeNetUpdates(
     const RealType& hLTrueValue, const RealType& hRTrueValue,
     const RealType& huLTrueValue, const RealType& huRTrueValue,
     const RealType& bLTrueValue, const RealType& bRTrueValue,
     RealType& hNetUpdateLeft,
     RealType& hNetUpdateRight,
     RealType& huNetUpdateLeft,
     RealType& huNetUpdateRight,
     RealType& maxEdgeSpeed);

   void applyBoundaryCondition(RealType& hL, RealType& hR, RealType& huL, RealType& huR, RealType& bL, RealType& bR);


 };

}

