#pragma once

#include <vector>
#include <deque>
#include "RTCore.h"
#include "UICore.h"

namespace Nf
{
  class EMNeedleTipCalibrator
  {
  protected:
    arma::mat m_R;
    arma::mat m_yx;
    arma::vec m_c;
    arma::mat m_tipFrame;

  public:
    EMNeedleTipCalibrator();
    void AddPoint(const Vec3d &emPoint, const Matrix33d &emTipFrame, const Vec3d &tipPoint);
    void ClearPoints();
    void DoCalibration();
    void GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame);
    void GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, const Vec3d &emPos, const Matrix33d &emFrame);
    void SetSolution(const arma::mat &solution);
  };
}