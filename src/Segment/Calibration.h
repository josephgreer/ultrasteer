#pragma once

#include <vector>
#include <deque>
#include "RTCore.h"
#include "UICore.h"

namespace Nf
{
  class Calibrator
  {
  public:
    virtual void ClearPoints() = 0;
    virtual void DoCalibration() = 0;
    virtual void ResetSolution() = 0;
  };

  class EMNeedleTipCalibrator : public Calibrator
  {
  protected:
    arma::mat m_R;
    arma::mat m_yx;
    arma::vec m_c;
    arma::mat m_tipFrame;

  public:
    EMNeedleTipCalibrator();
    virtual void AddPoint(const Vec3d &emPoint, const Matrix33d &emTipFrame, const Vec3d &tipPoint);
    virtual void ClearPoints();
    virtual void DoCalibration();
    void GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame) const;
    void GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, const Vec3d &emPos, const Matrix33d &emFrame) const;
    void SetSolution(const arma::mat &solution);
    virtual void ResetSolution();
  };

  class NeedleCurvatureCalibrator : public Calibrator
  {
  protected:
    arma::mat m_pts;

  public:
    NeedleCurvatureCalibrator();
    virtual void AddPoint(const Vec3d &point);
    virtual void ClearPoints();
    virtual void DoCalibration();
    virtual void ResetSolution();
  };
}