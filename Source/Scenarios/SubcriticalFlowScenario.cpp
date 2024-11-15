/**
 * @file SubcriticalFlowScenario.cpp
 *
 * @author Group 2 (Jonathan)
 * @date 01.11.2024
 */

#include "SubcriticalFlowScenario.hpp"


Scenarios::SubcriticalFlowScenario::SubcriticalFlowScenario(unsigned int size):
  size_(size) {}

RealType Scenarios::SubcriticalFlowScenario::getCellSize() const { return 25.0 / size_; } //TODO RealType(25.0)?

RealType Scenarios::SubcriticalFlowScenario::getHeight(unsigned int pos) const {
  return -getBathymetry(pos);
}

RealType Scenarios::SubcriticalFlowScenario::getMomentum(unsigned int pos) const {
  (void) pos;
  return 4.42;
}

RealType Scenarios::SubcriticalFlowScenario::getBathymetry(unsigned int pos) const {
  if (pos <= size_ * 8 / 25 || pos >= size_ * 12 / 25) { 
    return -2;
  }

  //erstmal Zwischenschritte berechnen, da es davor zu Fehlern kam
  // können wir noch zusammensetzen 
  double p = (RealType(pos) / size_ * 25.0 - 10.0) * (RealType(pos) / size_ * 25.0 - 10.0);
  double p_ = (0.05 * p);
  return -1.8 - p_;
}

