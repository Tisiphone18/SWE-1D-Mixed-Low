/**
* @file WavePropagationBlockBFLoat.cpp
*
* original author: Sebastian Rettenberger <rettenbs@in.tum.de>
* modified by: Lena Elisabeth Holtmannspötter
*/

#include "WavePropagationBlockBFLoat.hpp"

Blocks::WavePropagationBlockBFloat::WavePropagationBlockBFloat(ComputeType* h, ComputeType*hu, RealType* b, unsigned int size, ComputeType cellSize):
 h_(h),
 hu_(hu),
 b_(b),
 size_(size),
 cellSize_(cellSize),
 leftBoundary_(OutflowBoundary),
 rightBoundary_(OutflowBoundary) {

 // Allocate net updates
 hNetUpdatesLeft_   = new ComputeType[size + 1];
 hNetUpdatesRight_  = new ComputeType[size + 1];
 huNetUpdatesLeft_  = new RealType[size + 1];
 huNetUpdatesRight_ = new RealType[size + 1];
}

Blocks::WavePropagationBlockBFloat::~WavePropagationBlockBFloat() {
 // Free allocated memory
 delete[] hNetUpdatesLeft_;
 delete[] hNetUpdatesRight_;
 delete[] huNetUpdatesLeft_;
 delete[] huNetUpdatesRight_;
}

ComputeType Blocks::WavePropagationBlockBFloat::computeNumericalFluxes() {

 //std::cout << "\n[computeNumericalFluxes] Start\n";

 ComputeType maxWaveSpeed = ComputeType(0.0);

 // Loop over all edges
 for (unsigned int i = 1; i < size_ + 2; i++) {
   ComputeType maxEdgeSpeed = ComputeType(0.0);

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

   //std::cout << " Edge " << i-1 << "->" << i
   //          << " h_L=" << h_[i-1] << " h_R=" << h_[i]
   //          << " hu_L=" << hu_[i-1] << " hu_R=" << hu_[i]
   //          << " b_L=" << b_[i-1] << " b_R=" << b_[i]
   //          << " NetLeft(h,hu)=(" << hNetUpdatesLeft_[i-1] << "," << huNetUpdatesLeft_[i-1] << ")"
   //          << " NetRight(h,hu)=(" << hNetUpdatesRight_[i-1] << "," << huNetUpdatesRight_[i-1] << ")"
   //          << " maxEdgeSpeed=" << maxEdgeSpeed
   //          << "\n";

   // Update maxWaveSpeed
   if (maxEdgeSpeed > maxWaveSpeed) maxWaveSpeed = maxEdgeSpeed;
 }
 //std::cout << "[computeNumericalFluxes] maxWaveSpeed=" << maxWaveSpeed << "\n";

 // TODO back to 0.4 Compute CFL condition
 ComputeType maxTimeStep = maxWaveSpeed > ComputeType(0.0) ? ComputeType(cellSize_) / maxWaveSpeed * ComputeType(0.4) : std::numeric_limits<ComputeType>::max();

 //std::cout << "[computeNumericalFluxes] maxTimeStep=" << maxTimeStep << "\n";

 return maxTimeStep;
}

void Blocks::WavePropagationBlockBFloat::updateUnknowns(ComputeType dt) {
 // Precompute dt/dx in high precision
 const ComputeType invDx = ComputeType(1) / ComputeType(cellSize_);
 const ComputeType dtOverDx = ComputeType(dt) * invDx;

 //std::cout << "\n[updateUnknowns] dt=" << dt << " dt/dx=" << dtOverDx << "\n";

 for (unsigned int i = 1; i < size_ + 1; i++) {
   // Load as high precision
   ComputeType h_i  = static_cast<ComputeType>(h_[i]);
   ComputeType hu_i = static_cast<ComputeType>(hu_[i]);

   // Neighbor net updates loaded & summed in high precision
   const ComputeType dH  =
     static_cast<ComputeType>(hNetUpdatesRight_[i - 1]) +
     static_cast<ComputeType>(hNetUpdatesLeft_[i]);

   const ComputeType dHU =
     static_cast<ComputeType>(huNetUpdatesRight_[i - 1]) +
     static_cast<ComputeType>(huNetUpdatesLeft_[i]);

   /*std::cout << " Cell : " << i << " with dt : " << static_cast<float>(dt) << " and dt/dx : " << static_cast<float>(dtOverDx)
             << " h=" << static_cast<float>(h_i) << " hu=" << static_cast<float>(hu_i)
             << " dH=" << static_cast<float>(hNetUpdatesRight_[i - 1]) << " + " << static_cast<float>(hNetUpdatesLeft_[i]) << " = " << static_cast<float>(dH)
             << " dHU=" << static_cast<float>(huNetUpdatesRight_[i - 1]) << " + " << static_cast<float>(huNetUpdatesLeft_[i]) << " = " << static_cast<float>(dHU)  << "\n";
  */

   h_i  = fma_compute(-dtOverDx, dH,  h_i);
   hu_i = fma_compute(-dtOverDx, dHU, hu_i);

   if (h_i < ComputeType(0.0)) {
     h_i  = ComputeType(0.0);
     hu_i = ComputeType(0.0);
   }

   h_[i]  = h_i;
   hu_[i] = hu_i;

   //std::cout << " -> new h=" << h_[i] << " hu=" << hu_[i] << "\n";
 }
}

void Blocks::WavePropagationBlockBFloat::applyBoundaryConditions() {
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

void Blocks::WavePropagationBlockBFloat::setRightBoundaryCondition(BoundaryCondition condition) {
 rightBoundary_ = condition;
}

void Blocks::WavePropagationBlockBFloat::setLeftBoundaryCondition(BoundaryCondition condition) {
 leftBoundary_ = condition;
}
