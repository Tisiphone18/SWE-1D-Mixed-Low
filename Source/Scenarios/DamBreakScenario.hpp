/**
 * @headerfile DamBreakScenario.hpp
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

#pragma once

#include "Scenario.hpp"
#include "Tools/RealType.hpp"

namespace Scenarios {

  class DamBreakScenario: public Scenario {
    /** Width of space to be simulated */
    const RealType width_;
    /** Number of cells */
    const unsigned int size_;
    /** Initial height on left side of dam */
    const RealType hL_;
    /** Initial height on right side of dam */
    const RealType hR_;
    /** Initial particle speed on right side of dam */
    const RealType uR_;

  public:
    /**
     * Constructor of DamBreakScenario
     *
     * @param width Width of space to be simulated
     * @param size Number of cells
     * @param hL Initial height on left side of dam
     * @param hR Initial height on right side of dam
     * @param uR Initial particle speed on right side of dam
     */
DamBreakScenario(RealType width, unsigned int size, RealType hL, RealType hR, RealType uR);
    ~DamBreakScenario() override = default;

    /**
    * @return Cell size of one cell (= domain size/number of cells)
    */
    RealType getCellSize() const override;

    /**
     * @return Initial water height at pos
     */
    RealType getHeight(unsigned int pos) const override;

    /**
     * @return Initial momentum of water (hu) at position pos
     */
    RealType getMomentum(unsigned int pos) const override;

    /**
     * @return Bathymetry (b) at position pos
     */
    RealType getBathymetry(unsigned int pos) const override;
  };

} // namespace Scenarios
