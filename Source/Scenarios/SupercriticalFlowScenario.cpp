/**
 * @file SupercriticalFlowScenario.cpp
 *
 * @author Group 2 (Jonathan)
 * @date 01.11.2024
 */

#include "SupercriticalFlowScenario.hpp"

Scenarios::SupercriticalFlowScenario::SupercriticalFlowScenario(unsigned int size):
  size_(size) {}

RealType Scenarios::SupercriticalFlowScenario::getCellSize() const { return 25.0 / size_; } // TODO RealType(25.0)?

RealType Scenarios::SupercriticalFlowScenario::getHeight(unsigned int pos) const {
  (void) pos;
  return -getBathymetry(pos);
}

RealType Scenarios::SupercriticalFlowScenario::getMomentum(unsigned int pos) const {
  (void) pos;
  return 0.18;
}

RealType Scenarios::SupercriticalFlowScenario::getBathymetry(unsigned int pos) const {
  if (pos <= size_ * 8 / 25 || pos >= size_ * 12 / 25) { 
    return -0.33;
  }

  //erstmal Zwischenschritte berechnen, da es davor zu Fehlern kam
  // können wir noch zusammensetzen 
  double p = (RealType(pos) / size_ * 25.0 - 10.0) * (RealType(pos) / size_ * 25.0 - 10.0);
  double p_ = (0.05 * p);
  return -0.13 - p_;
}

