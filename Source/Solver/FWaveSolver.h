#ifndef FWAVESOLVER_H
#define FWAVESOLVER_H

class FWaveSolver {
public:
  void computeNetUpdates(
    double hL, double hR,
    double huL, double huR,
    double bL, double bR,
    double& hNetUpdateLeft,
    double& hNetUpdateRight,
    double& huNetUpdateLeft,
    double& huNetUpdateRight,
    double& maxEdgeSpeed);
};

#endif // FWAVESOLVER_H
