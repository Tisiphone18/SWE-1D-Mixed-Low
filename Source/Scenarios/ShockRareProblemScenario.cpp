//
// Created by johan on 20.10.24.
//

#include "ShockRareProblemScenario.hpp"

#include <cassert>

Scenarios::ShockRareProblemScenario::ShockRareProblemScenario(const unsigned int size, const unsigned int pos_of_problem, const RealType h, const RealType uL):
  size_(size),
  pos_of_problem_(pos_of_problem),
  h_(h),
  uL_(uL) {assert(h > 0); assert(uL != 0);}

RealType Scenarios::ShockRareProblemScenario::getCellSize() const { return RealType(1000) / size_; }

RealType Scenarios::ShockRareProblemScenario::getHeight() const {
  return h_;
}

RealType Scenarios::ShockRareProblemScenario::getSpeed(unsigned int pos) const {
  if (pos <= pos_of_problem_) {
    return uL_; // TODO hL*uL??
  }
  return -uL_; // TODO hL*-uL??
}

