/**
 * @file SupercriticalFlowScenario.cpp
 *
 * @author Group 2 (Jonathan)
 * @date 01.11.2024
 */


#include <cassert>
#include "SupercriticalFlowScenario.hpp"
#include <iostream>

Scenarios::SupercriticalFlowScenario::SupercriticalFlowScenario(unsigned int size):
  size_(size){}

RealType Scenarios::SupercriticalFlowScenario::getCellSize() const { return 25.0 / size_; }

RealType Scenarios::SupercriticalFlowScenario::getHeight(unsigned int pos) const {
    //assert x between 0, 25 ?
  return -getBathymetry(pos);
}

RealType Scenarios::SupercriticalFlowScenario::getMomentum(unsigned int pos) const {
  //assert x between 0, 25 ?
  return 0.18;
}

RealType Scenarios::SupercriticalFlowScenario::getBathymetry(unsigned int pos) const {
  if (pos <= 8 || pos >=  12) {
    return -0.33;
  }

  //erstmal Zwischenschritte berechnen, da es davor zu Fehlern kam
  // können wir noch zusammensetzen 
  double p = (pos - 10) * (pos - 10);
  double p_ = (0.05 * p);
  return -0.13 - p_;
}

