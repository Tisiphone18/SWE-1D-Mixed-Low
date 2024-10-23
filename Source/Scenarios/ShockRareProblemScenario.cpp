//
// Created by johan on 20.10.24.
//

#include "ShockRareProblemScenario.hpp"

#include <cassert>

Scenarios::ShockRareProblemScenario::ShockRareProblemScenario(const unsigned int size, const unsigned int pos_of_problem, const RealType h, const RealType huL):
  size_(size),
  pos_of_problem_(pos_of_problem),
  h_(h),
  huL_(huL) {assert(h > 0); assert(huL != 0); assert(pos_of_problem_ < size_);}

RealType Scenarios::ShockRareProblemScenario::getCellSize() const { return RealType(1000) / size_; }

RealType Scenarios::ShockRareProblemScenario::getHeight() const {
  return h_;
}

RealType Scenarios::ShockRareProblemScenario::getMomentum(unsigned int pos) const {
  if (pos <= pos_of_problem_) {
    return huL_;
  }
  return -huL_;
}

