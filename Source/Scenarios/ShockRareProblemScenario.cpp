/**
 * @file
 */
//
// Created by johan on 20.10.24.
//

#include "ShockRareProblemScenario.hpp"

#include <cassert>

Scenarios::ShockRareProblemScenario::ShockRareProblemScenario(const RealType width, const unsigned int size, const unsigned int pos_of_problem, const RealType h, const RealType huL):
  width_(width),
  size_(size),
  pos_of_problem_(pos_of_problem),
  h_(h),
  huL_(huL) {
    assert(width_ > 0.0 && "simulation width must be greaterthan zero");
    assert(h > 0.0 && "height must be greater than zero");
    assert(huL != 0.0 && "momentum cannot be zero");
    assert(pos_of_problem_ < size_ && "position of problem must be in [0, size)");
}

RealType Scenarios::ShockRareProblemScenario::getCellSize() const { return width_ / size_; }

RealType Scenarios::ShockRareProblemScenario::getHeight(unsigned int pos) const {
  return h_;
}

RealType Scenarios::ShockRareProblemScenario::getMomentum(unsigned int pos) const {
  if (pos <= pos_of_problem_) {
    return huL_;
  }
  return -huL_;
}

