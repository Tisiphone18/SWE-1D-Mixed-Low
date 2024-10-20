//
// Created by johan on 20.10.24.
//
#pragma once

#include "Tools/RealType.hpp"

namespace Scenarios {

  class ShockRareProblemScenario {
    /** Number of cells */
    const unsigned int size_;
    const unsigned int pos_of_problem_;
    const RealType h_;
    const RealType uL_;

  public:
    /**
     *
     * @param size
     * @param pos_of_problem
     * @param h constant water height
     * @param uL particle speed on the left side of the position of the problem; < 0 => RareRareProblem; > 0 => ShockShockProblem
     */
ShockRareProblemScenario(unsigned int size, unsigned int pos_of_problem, RealType h, RealType uL);
    ~ShockRareProblemScenario() = default;

    /**
     * @return Cell size of one cell (= domain size/number of cells)
     */
    RealType getCellSize() const;

    /**
     * @return Initial water height
     */
    RealType getHeight() const;

    /**
     * @return Initial particle speed at position pos
     */
    RealType getSpeed(unsigned int pos) const;
  };

} // namespace Scenarios
