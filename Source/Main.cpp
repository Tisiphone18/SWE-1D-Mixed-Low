/**
 * @file Main.cpp
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

#include <cstring>
#include <fenv.h>

#include "Blocks/WavePropagationBlock.hpp"
#include "Scenarios/DamBreakScenario.hpp"
#include "Scenarios/Scenario.hpp"
#include "Scenarios/ShockRareProblemScenario.hpp"
#include "Scenarios/SubcriticalFlowScenario.hpp"
#include "Scenarios/SupercriticalFlowScenario.hpp"
#include "Tools/Args.hpp"
#include "Tools/Logger.hpp"
#include "Tools/RealType.hpp"
#include "Writers/ConsoleWriter.hpp"
#include "Writers/VTKWriter.hpp"

int main(int argc, char** argv) {
  // Triggers signals on floating point errors, i.e. prohibits quiet NaNs and alike.
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);

  // Parse command line parameters
  Tools::Args args(argc, argv);

  // Scenario
  Scenarios::Scenario* scenario;
  switch (args.getScenarioName()) {
    default: // implicitly case 'D' as well
      scenario = new Scenarios::DamBreakScenario(args.getWidth(), args.getSize(), args.getHL(), args.getHR(), args.getUR());
      break;
    case 'S':
      scenario = new Scenarios::ShockRareProblemScenario(args.getWidth(), args.getSize(), args.getSize()/2, args.getHL(), args.getHuL());
      break;
    case 'P':
      scenario = new Scenarios::SupercriticalFlowScenario(args.getSize());
      break;
    case 'B':
      scenario = new Scenarios::SubcriticalFlowScenario(args.getSize());
      break;
  }

  // Allocate memory
  // Water height
  RealType* h = new RealType[args.getSize() + 2];
  // Momentum
  RealType* hu = new RealType[args.getSize() + 2];
  // Bathymetry
  RealType* b = new RealType[args.getSize() + 2];

  // Initialize water height and momentum
  for (unsigned int i = 0; i < args.getSize() + 2; i++) {
    h[i] = scenario->getHeight(i);
    hu[i] = scenario->getMomentum(i);
    b[i] = scenario->getBathymetry(i);
  }

  // Create a writer that is responsible printing out values
  Writers::ConsoleWriter consoleWriter;
  Writers::VTKWriter     vtkWriter("SWE1D", scenario->getCellSize());

  // Helper class computing the wave propagation
  Blocks::WavePropagationBlock wavePropagation(h, hu, b, args.getSize(), scenario->getCellSize());

  // Write initial data
  Tools::Logger::logger.info("Initial data");

  // Current time of simulation
  double t = 0;

  // consoleWriter.write(h, hu, args.getSize());
  vtkWriter.write(t, h, hu, b, args.getSize());

  for (unsigned int i = 0; i < args.getTimeSteps(); i++) {
    // Do one time step

    // Update boundaries
    wavePropagation.applyBoundaryConditions();

    // Compute numerical flux on each edge
    RealType maxTimeStep = wavePropagation.computeNumericalFluxes();

    // Update unknowns from net updates
    wavePropagation.updateUnknowns(maxTimeStep);

    Tools::Logger::logger
      << "Computing iteration " << i << " at time " << t << " with max. timestep " << maxTimeStep << std::endl;

    // Update time
    t += maxTimeStep;

    // Write new values
    // consoleWriter.write(h, hu, args.getSize());
    vtkWriter.write(t, h, hu, b, args.getSize());
  }

  // Free allocated memory
  delete[] h;
  delete[] hu;
  delete[] b;
  delete scenario;

  return EXIT_SUCCESS;
}
