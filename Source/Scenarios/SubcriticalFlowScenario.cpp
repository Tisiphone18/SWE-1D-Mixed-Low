/**
 * @file SubcriticalFlowScenario.cpp
 *
 * @author Group 2 (Jonathan)
 * @date 01.11.2024
 */

#include <cassert>
#include "SubcriticalFlowScenario.hpp"
#include <iostream>

Scenarios::SubcriticalFlowScenario::SubcriticalFlowScenario(unsigned int size):
  size_(size) {}

RealType Scenarios::SubcriticalFlowScenario::getCellSize() const { return 25.0 / size_; } //TODO RealType(25.0)?

RealType Scenarios::SubcriticalFlowScenario::getHeight(unsigned int pos) const {
    // TODO assert x \el [0,size_]? imo the user should be aware of size_, as this is passed onto the constructor
  return -getBathymetry(pos);
}

RealType Scenarios::SubcriticalFlowScenario::getMomentum(unsigned int pos) const {
  // TODO assert x \el [0,size_]? imo the user should be aware of size_, as this is passed onto the constructor
  return 4.42;
}

RealType Scenarios::SubcriticalFlowScenario::getBathymetry(unsigned int pos) const {
  if (pos <= 8 || pos >=  12) { // TODO this is wrong, size_ != spacial domain!: pos <= size_ * 8 / 25 || pos >= size_ * 12 / 25
    return -2;
  }

  //erstmal Zwischenschritte berechnen, da es davor zu Fehlern kam
  // können wir noch zusammensetzen 
  double p = (pos - 10) * (pos - 10);
  double p_ = (0.05 * p);
  return -1.8 - p_;
}

