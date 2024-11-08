/**
 * @file WavePropagationBlock.cpp
 *  This file is part of SWE1D
 *
 *  SWE1D is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SWE1D is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SWE1D.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von SWE1D.
 *
 *  SWE1D ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  SWE1D wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 * @copyright 2013 Technische Universitaet Muenchen
 * @author Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#include "WavePropagationBlock.hpp"

Blocks::WavePropagationBlock::WavePropagationBlock(RealType* h, RealType* hu, RealType* b, unsigned int size, RealType cellSize):
  h_(h),
  hu_(hu),
  b_(b),
  size_(size),
  cellSize_(cellSize),
  leftBoundary_(OutflowBoundary),
  rightBoundary_(OutflowBoundary) {

  // Allocate net updates
  hNetUpdatesLeft_   = new RealType[size + 1];
  hNetUpdatesRight_  = new RealType[size + 1];
  huNetUpdatesLeft_  = new RealType[size + 1];
  huNetUpdatesRight_ = new RealType[size + 1];
}

Blocks::WavePropagationBlock::~WavePropagationBlock() {
  // Free allocated memory
  delete[] hNetUpdatesLeft_;
  delete[] hNetUpdatesRight_;
  delete[] huNetUpdatesLeft_;
  delete[] huNetUpdatesRight_;
}

RealType Blocks::WavePropagationBlock::computeNumericalFluxes() {
  RealType maxWaveSpeed = RealType(0.0);

  // Loop over all edges
  for (unsigned int i = 1; i < size_ + 2; i++) {
    RealType maxEdgeSpeed = RealType(0.0);

    // Compute net updates
    solver_with_bathymetry_.computeNetUpdates(
      h_[i - 1],
      h_[i],
      hu_[i - 1],
      hu_[i],
      b_[i - 1],
      b_[i],
      hNetUpdatesLeft_[i - 1],
      hNetUpdatesRight_[i - 1],
      huNetUpdatesLeft_[i - 1],
      huNetUpdatesRight_[i - 1],
      maxEdgeSpeed
    );
    // Update maxWaveSpeed
    if (maxEdgeSpeed > maxWaveSpeed) {
      maxWaveSpeed = maxEdgeSpeed;
    }
  }

  

  // Compute CFL condition
  RealType maxTimeStep = maxWaveSpeed > 0.0 ? cellSize_ / maxWaveSpeed * RealType(0.4) : std::numeric_limits<RealType>::max();

  return maxTimeStep;
}

void Blocks::WavePropagationBlock::updateUnknowns(RealType dt) {
  // Loop over all inner cells
  for (unsigned int i = 1; i < size_ + 1; i++) {
    h_[i] -= dt / cellSize_ * (hNetUpdatesRight_[i - 1] + hNetUpdatesLeft_[i]);
    hu_[i] -= dt / cellSize_ * (huNetUpdatesRight_[i - 1] + huNetUpdatesLeft_[i]);
  }
}

void Blocks::WavePropagationBlock::applyBoundaryConditions() {
  if (leftBoundary_ == OutflowBoundary) {
    h_[0]         = h_[1];
    hu_[0]         = hu_[1];
    b_[0]         = b_[1];
  } else if (leftBoundary_ == ReflectingBoundary) {
    h_[0]         = h_[1];
    hu_[0]        = -hu_[1];
    b_[0]         = b_[1];
  }

  if (rightBoundary_ == OutflowBoundary) {
    h_[size_ + 1]         = h_[size_];
    hu_[size_ + 1]         = hu_[size_];
    b_[size_ + 1]         = b_[size_];
  } else if (rightBoundary_ == ReflectingBoundary) {
    h_[size_ + 1]         = h_[size_];
    hu_[size_ + 1]        = -hu_[size_];
    b_[size_ + 1]         = b_[size_];
  }
}

void Blocks::WavePropagationBlock::setRightBoundaryCondition(BoundaryCondition condition) {
  rightBoundary_ = condition;
}

void Blocks::WavePropagationBlock::setLeftBoundaryCondition(BoundaryCondition condition) {
  leftBoundary_ = condition;
}
