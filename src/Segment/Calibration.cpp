#pragma once

#include "Calibration.h"

using namespace arma;
namespace Nf
{
  using ::s32;
  using namespace arma;

  ////////////////////////////////////////////////////////////////
  //// BEGIN EMNeedleTipCalibrator
  ////////////////////////////////////////////////////////////////
  EMNeedleTipCalibrator::EMNeedleTipCalibrator()
  {
    m_R.clear();
    m_yx.clear();
    m_c.clear();
    m_tipFrame.clear();
  }

  void EMNeedleTipCalibrator::ClearPoints()
  {
    m_R.clear();
    m_yx.clear();
    m_c.clear();
  }

  void EMNeedleTipCalibrator::AddPoint(const Vec3d &emPoint, const Matrix33d &emTipFrame, const Vec3d &tipPoint)
  {
    m_R = join_vert(m_R,emTipFrame.ToArmaMat());
    m_yx = join_vert(m_yx, (tipPoint-emPoint).ToArmaVec());
  }

  void EMNeedleTipCalibrator::DoCalibration()
  {
    m_c = solve(m_R,m_yx);

    m_tipFrame = eye<mat>(3,3);

    vec zaxis; zaxis << 1 << endr << 0 << endr << 0;
    vec yaxis = m_c; yaxis(0) = 0;
    yaxis = yaxis/norm(yaxis);
    vec xaxis = cross(yaxis,zaxis);
    m_tipFrame.submat(span(0,2), span(0,0)) = xaxis;
    m_tipFrame.submat(span(0,2), span(1,1)) = yaxis;
    m_tipFrame.submat(span(0,2), span(2,2)) = zaxis;

  }

  void EMNeedleTipCalibrator::GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame) const
  {
    if(m_c.n_rows == 0 || m_c.n_cols == 0) {
      tipOffset = Vec3d(0,0,0);
      tipFrame = Matrix33d::I();
      return;
    } 
    tipOffset = Vec3d::FromArmaVec(m_c);
    tipFrame = Matrix33d::FromArmaMatrix3x3(m_tipFrame);
  }


  void EMNeedleTipCalibrator::GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, const Vec3d &emPos, const Matrix33d &emFrame) const
  {
    if(m_c.n_rows == 0 || m_c.n_cols == 0) {
      tipOffset = emPos;
      tipFrame = emFrame;
      return;
    } 
    tipOffset = emPos+emFrame.Col(0)*m_c(0,0)+emFrame.Col(1)*m_c(1,0)+emFrame.Col(2)*m_c(2,0);

    tipFrame = emFrame*Matrix33d::FromArmaMatrix3x3(m_tipFrame);
  }

  void EMNeedleTipCalibrator::SetSolution(const arma::mat &solution)
  {
    m_tipFrame = solution.submat(span(0,2), span(0,2));
    m_c = (vec)(solution.submat(span(3,3), span(0,2)).t());
  }

  void EMNeedleTipCalibrator::ResetSolution()
  {
    m_c.clear();
    m_tipFrame.clear();
  }

  bool EMNeedleTipCalibrator::IsCalibrated()
  {
    return m_c.n_elem > 0 && m_tipFrame.n_elem > 0;
  }
  ////////////////////////////////////////////////////////////////
  //// END EMNeedleTipCalibrator
  ////////////////////////////////////////////////////////////////

  
  ////////////////////////////////////////////////////////////////
  //// BEGIN NeedleCurvatureCalibrator
  ////////////////////////////////////////////////////////////////
  NeedleCurvatureCalibrator::NeedleCurvatureCalibrator()
  {
    m_pts.clear();
  }
  void NeedleCurvatureCalibrator::AddPoint(const Vec3d &point)
  {
  }

  void NeedleCurvatureCalibrator::ClearPoints()
  {
    m_pts.clear();
  }

  void NeedleCurvatureCalibrator::DoCalibration()
  {
  }

  void NeedleCurvatureCalibrator::ResetSolution()
  {
    m_pts.clear();
  }

  bool NeedleCurvatureCalibrator::IsCalibrated()
  {
    return m_pts.n_elem > 0;
  }
  ////////////////////////////////////////////////////////////////
  //// END NeedleCurvatureCalibrator
  ////////////////////////////////////////////////////////////////
  
  ////////////////////////////////////////////////////////////////
  //// BEGIN PlaneCalibrator
  ////////////////////////////////////////////////////////////////
  PlaneCalibrator::PlaneCalibrator()
  {
    m_pts.clear();
    m_plane = Plane();
  }

  void PlaneCalibrator::AddPoint(const Vec3d &point)
  {
    m_pts = join_vert(m_pts, point.ToArmaVec().t());
  }
  void PlaneCalibrator::SetSolution(const arma::mat &solution)
  {
    m_plane = solution;
  }

  void PlaneCalibrator::ClearPoints()
  {
    m_pts.clear();
    m_plane = Plane();
  }

  void PlaneCalibrator::DoCalibration()
  {
    arma::mat origin = mean(m_pts,0).t();
    arma::mat pts = m_pts.t() - repmat(origin, 1, m_pts.n_rows);
    arma::mat U, V;
    arma::vec s;
    svd(U, s, V, pts);

    arma::vec n = U.col(2);
    f64 d = -dot(n,origin);
    f64 normn = norm(n);
    d = d/normn;
    n = n/normn;
    arma::mat abcd = zeros(4,1);
    abcd.submat(arma::span(0,2), arma::span(0,0)) = n;
    abcd(3) = d;
    m_plane = Plane(abcd);
  }

  void PlaneCalibrator::ResetSolution()
  {
    m_pts.clear();
    m_plane = Plane();
  }

  bool PlaneCalibrator::IsCalibrated()
  {
    return !m_plane.IsZero();
  }
  ////////////////////////////////////////////////////////////////
  //// END PlaneCalibrator
  ////////////////////////////////////////////////////////////////
}