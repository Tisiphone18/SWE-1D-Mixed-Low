#ifndef FWAVESOLVERV2_H
#define FWAVESOLVERV2_H

#include "Tools/RealType.hpp"

namespace Solvers {
  class FWaveSolverStudentV2 {
  public:
    void computeNetUpdates(
      const RealType &hL, const RealType &hR,
      const RealType &huL, const RealType &huR,
      const RealType &bL, const RealType &bR,
      RealType& hNetUpdateLeft,
      RealType& hNetUpdateRight,
      RealType& huNetUpdateLeft,
      RealType& huNetUpdateRight,
      RealType& maxEdgeSpeed);
  };
}

#endif // FWAVESOLVERV2_H
