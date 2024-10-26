/**
 * @file
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

#include "DamBreakScenario.hpp"

#include <cassert>

Scenarios::DamBreakScenario::DamBreakScenario(const RealType width, unsigned int size, RealType hL, RealType hR, RealType uR):
  width_(width),
  size_(size),
  hL_(hL),
  hR_(hR),
  uR_(uR) {
    assert(width_ > 0.0 && "simulation width must be greaterthan zero");
    assert(hL_ > hR_ && "height on left side of dam must be greater than height on right side of dam");
    assert(hR_ > 0.0 && "height on right side of dam must be greater than zero");
    assert(uR_ >= 0.0 && "particle speed on right side of dam must be greater than zero");
}

RealType Scenarios::DamBreakScenario::getCellSize() const { return width_ / size_; }

RealType Scenarios::DamBreakScenario::getHeight(unsigned int pos) const {
  if (pos <= size_ / 2) {
    return hL_;
  }
  return hR_;
}

RealType Scenarios::DamBreakScenario::getMomentum(unsigned int pos) const {
  if (pos <= size_ / 2) {
    return 0.0;
  }
  return hR_*uR_;
}
