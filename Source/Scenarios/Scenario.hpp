/**
 * @file
 */
// 
// Created by johan on 23.10.24.
// 

#pragma once

#include "Tools/RealType.hpp"

namespace Scenarios {

  class Scenario {

  public:
    virtual ~Scenario() = default;

    /**
     * @return Cell size of one cell (= domain size/number of cells)
     */
    virtual RealType getCellSize() const = 0;

    /**
     * @return Initial water height at pos
     */
    virtual RealType getHeight(unsigned int pos) const = 0;

    /**
     * @return Initial momentum of water (hu) at position pos
     */
    virtual RealType getMomentum(unsigned int pos) const = 0;
    /**
     * @return Bathymetry (b) at position pos
     */
    virtual RealType getBathymetry(unsigned int pos) const {
      return 0;
    };
  };

} // namespace Scenarios