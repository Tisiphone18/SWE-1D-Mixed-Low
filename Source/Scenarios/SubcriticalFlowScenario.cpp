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
  size_(size){}

RealType Scenarios::SubcriticalFlowScenario::getCellSize() const { return RealType(100) / size_; }

RealType Scenarios::SubcriticalFlowScenario::getHeight(unsigned int pos) const {
    //assert x between 0, 25 ?
  return -getBathymetry(pos);
}

RealType Scenarios::SubcriticalFlowScenario::getMomentum(unsigned int pos) const {
  //assert x between 0, 25 ?
  return 4.42;
}

RealType Scenarios::SubcriticalFlowScenario::getBathymetry(unsigned int pos) const {
  if (pos <= 8 || pos >=  12) {
    return -2;
  }

  //erstmal Zwischenschritte berechnen, da es davor zu Fehlern kam
  // können wir noch zusammensetzen 
  double p = (pos - 10) * (pos - 10);
  double p_ = (0.05 * p);
  return -1.8 - p_;
}

