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
    const RealType huL_;

  public:
    /**
     *
     * @param size
     * @param pos_of_problem position where momentum changes direction
     * @param h constant water height
     * @param huL momentum of water on the left side of position of the problem; < 0 => RareRareProblem; > 0 => ShockShockProblem
     */
ShockRareProblemScenario(unsigned int size, unsigned int pos_of_problem, RealType h, RealType huL);
    ~ShockRareProblemScenario() = default;

    /**
     * @return Cell size of one cell (= domain size/number of cells)
     */
    RealType getCellSize() const;

    /**
     * @return Initial water height (h)
     */
    RealType getHeight() const;

    /**
     * @return Initial momentum of water (hu) at position pos
     */
    RealType getMomentum(unsigned int pos) const;
  };

} // namespace Scenarios
