/**
 * @file SubcriticalFlowScenario.hpp
 *
 * @author Group 2 (Jonathan)
 * @date 01.11.2024
 */


#pragma once

#include "Scenario.hpp"

namespace Scenarios {

  class SubcriticalFlowScenario: public Scenario {
    /** Number of cells */
    const unsigned int size_;


  public:
    /**
     * Constructor of SubcriticalFlowScenario
     *
     * @param size Number of cells
     */
    SubcriticalFlowScenario( unsigned int size);
    ~SubcriticalFlowScenario() override = default;

    /**
    * @return Cell size of one cell (= domain size/number of cells)
    */
    RealType getCellSize() const override;

    /**
     * @return Initial water height at pos
     */
    RealType getHeight(unsigned int pos) const override;

    /**
     * @return Initial momentum of water (hu) at position pos
     */
    RealType getMomentum(unsigned int pos) const override;

    /**
     * @return Bathymetry (b) at position pos
     */
    RealType getBathymetry(unsigned int pos) const override;

  };

  
  

} // namespace Scenarios
