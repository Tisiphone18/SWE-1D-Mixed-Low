/**
 * @file
 */
//
// Created by johan on 20.10.24.
//
#pragma once

#include "Scenario.hpp"
#include "Tools/RealType.hpp"

namespace Scenarios {

  class ShockRareProblemScenario: public Scenario {
    /** Width of space to be simulated */
    const RealType width_;
    /** Number of cells */
    const unsigned int size_;
    /** Location of change of wave direction */
    const unsigned int pos_of_problem_;
    /** Initial water height */
    const RealType h_;
    /** Initial momentum of wave on left side; huR is implicitly -huL */
    const RealType huL_;

  public:
    /**
     *
     * @param width space to be simulated
     * @param size number of cells
     * @param pos_of_problem position where momentum changes direction
     * @param h constant water height
     * @param huL momentum of water on the left side of position of the problem; < 0 => RareRareProblem; > 0 => ShockShockProblem
     */
ShockRareProblemScenario(RealType width, unsigned int size, unsigned int pos_of_problem, RealType h, RealType huL);
    ~ShockRareProblemScenario() = default;

    /**
     * @return Cell size of one cell (= domain size/number of cells)
     */
    RealType getCellSize() const override;

    /**
     * @return Initial water height (h) at position pos
     */
    RealType getHeight(unsigned int pos) const override;

    /**
     * @return Initial momentum of water (hu) at position pos
     */
    RealType getMomentum(unsigned int pos) const override;
  };

} // namespace Scenarios
