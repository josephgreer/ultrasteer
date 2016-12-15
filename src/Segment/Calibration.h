#pragma once

#include <vector>
#include <deque>
#include <map>
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
    virtual bool IsCalibrated() = 0;
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
    virtual void GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, s32 frame) const;
    virtual void GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, const Vec3d &emPos, const Matrix33d &emFrame, s32 frame = 1) const;
    virtual void SetSolution(const arma::mat &solution);
    virtual void ResetSolution();
    virtual bool IsCalibrated();
  };

  struct TipCalibData
  {
    arma::vec c;
    arma::mat tipFrame;
  };

  TipCalibData TipCalibDataFromArmaMat(arma::mat a);
  TipCalibData TipCalibDataFromFilename(const char *path);

  class EMNeedleTipMultiCalibrator : public EMNeedleTipCalibrator
  {
  protected:
    std::map < s32, TipCalibData > m_calibs;
    std::vector < arma::mat > m_Rs;
    std::vector < arma::mat > m_yxs;
    std::vector < s32 > m_keyFrames;

  public:
    EMNeedleTipMultiCalibrator();
    virtual void SetSolution(std::vector < std::string > paths, std::vector < s32 > frames);
    virtual void ClearSolution();
    void GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, s32 frame) const;
    void GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, const Vec3d &emPos, const Matrix33d &emFrame, s32 frame = -1) const;
    virtual void AddPoint(const Vec3d &emPoint, const Matrix33d &emTipFrame, const Vec3d &tipPoint, s32 frame);
    virtual bool IsCalibrated();
    virtual void ClearPoints();
    virtual void DoCalibration();
    virtual void ResetSolution();
    virtual void SaveSolution(const char *basePath, const char *extension);
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
    virtual bool IsCalibrated();
  };

  class PlaneCalibrator : public Calibrator
  {
  protected:
    arma::mat m_pts;
    Plane m_plane;

  public:
    PlaneCalibrator();
    virtual void AddPoint(const Vec3d &point);
    void SetSolution(const arma::mat &solution);
    virtual void ClearPoints();
    virtual void DoCalibration();
    virtual void ResetSolution();
    virtual bool IsCalibrated();
    virtual void SetPoints(arma::mat pts) { m_pts = pts; }
    virtual Plane GetSolution() const { return m_plane; }
    arma::mat GetPoints() { return m_pts; }
  };
}