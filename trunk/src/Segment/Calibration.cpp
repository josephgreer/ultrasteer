#pragma once

#include "Calibration.h"

using namespace arma;
namespace Nf
{
  using ::s32;
  using namespace arma;

  EMNeedleTipCalibrator::EMNeedleTipCalibrator()
  {
    m_R.clear();
    m_yx.clear();
    m_c.clear();
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

    vec xaxis; xaxis << 1 << endr << 0 << endr << 0;
    vec yaxis = m_c; yaxis(0) = 0;
    yaxis = yaxis/norm(yaxis);
    vec zaxis = cross(xaxis,yaxis);
    m_tipFrame.submat(span(0,2), span(0,0)) = xaxis;
    m_tipFrame.submat(span(0,2), span(1,1)) = yaxis;
    m_tipFrame.submat(span(0,2), span(2,2)) = zaxis;

  }

  void EMNeedleTipCalibrator::GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame)
  {
    tipOffset = Vec3d::FromArmaVec(m_c);
    tipFrame = Matrix33d::FromArmaMatrix3x3(m_tipFrame);
  }


  void EMNeedleTipCalibrator::GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, const Vec3d &emPos, const Matrix33d &emFrame)
  {
    tipOffset = emPos+emFrame.Col(0)*m_c(0,0)+emFrame.Col(1)*m_c(1,0)+emFrame.Col(2)*m_c(2,0);

    tipFrame = emFrame*Matrix33d::FromArmaMatrix3x3(m_tipFrame);
  }
}