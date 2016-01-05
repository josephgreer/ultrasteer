#pragma once

#include "Calibration.h"

using namespace arma;
namespace Nf
{
  using ::s32;
  using namespace arma;

  TipCalibData TipCalibDataFromArmaMat(arma::mat a)
  {
    TipCalibData res;
    res.tipFrame = a.submat(arma::span(0,2), arma::span(0,2));
    res.c = (arma::vec)(a.submat(arma::span(3,3), arma::span(0,2)).t());
    return res;
  }

  TipCalibData TipCalibDataFromFilename(const char *path)
  {
    mat temp;
    temp.load(path);

    return TipCalibDataFromArmaMat(temp);
  }

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

  void EMNeedleTipCalibrator::GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, s32 frame) const
  {
    if(m_c.n_rows == 0 || m_c.n_cols == 0) {
      tipOffset = Vec3d(0,0,0);
      tipFrame = Matrix33d::I();
      return;
    } 
    tipOffset = Vec3d::FromArmaVec(m_c);
    tipFrame = Matrix33d::FromArmaMatrix3x3(m_tipFrame);
  }


  void EMNeedleTipCalibrator::GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, const Vec3d &emPos, const Matrix33d &emFrame, s32 frame) const
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
    TipCalibData data = TipCalibDataFromArmaMat(solution);
    m_tipFrame = data.tipFrame;
    m_c = data.c;
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
  //// BEGIN EMNeedleTipMultiCalibrator
  ////////////////////////////////////////////////////////////////
  EMNeedleTipMultiCalibrator::EMNeedleTipMultiCalibrator()
    : EMNeedleTipCalibrator()
  {
    m_calibs.clear();
    m_Rs.clear();
    m_yxs.clear();
  }


  void EMNeedleTipMultiCalibrator::GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, s32 frame) const
  {
    if(m_calibs.empty()) {
      tipOffset = Vec3d(0,0,0);
      tipFrame = Matrix33d::I();
      return;
    } 

    //Find closest frames
    std::map<s32, TipCalibData>::const_iterator lower = m_calibs.lower_bound(frame);
    std::map<s32, TipCalibData>::const_iterator upper = m_calibs.upper_bound(frame);

    s32 frameA,frameB; frameA = frameB = -1;
    f64 interp;
    if(lower == m_calibs.end() || upper == m_calibs.end()) {
      if(upper == m_calibs.end() && lower == m_calibs.end()) {
        upper = m_calibs.end(); upper--;
        lower = m_calibs.end(); lower--;
      } else if(upper == m_calibs.end()) {
        upper = lower;
      } else {
        lower = upper;
      }
      interp = 0;
    }
    else {
      if(lower->first != frame && lower != m_calibs.begin())
        lower--;
      interp = ((f64)frame-lower->first)/((f64)upper->first-lower->first);
    }

    arma::vec lc = lower->second.c;
    arma::vec uc = upper->second.c;
    arma::mat lf = lower->second.tipFrame;
    arma::mat uf = upper->second.tipFrame;

    arma::mat iframe = SO3Exp(interp*SO3Log(uf*lf.t()))*lf;
    arma::vec c = lc+interp*(uc-lc);

    tipOffset = Vec3d::FromArmaVec(c);
    tipFrame = Matrix33d::FromArmaMatrix3x3(iframe);
  }

  void EMNeedleTipMultiCalibrator::GetSolution(Vec3d &tipOffset, Matrix33d &tipFrame, const Vec3d &emPos, const Matrix33d &emFrame, s32 frame) const
  {
    Vec3d cc; Matrix33d iiframe;
    GetSolution(cc, iiframe, frame);

    arma::vec c = cc.ToArmaVec();
    arma::mat iframe = iiframe.ToArmaMat();
    
    tipOffset = emPos+emFrame.Col(0)*c(0,0)+emFrame.Col(1)*c(1,0)+emFrame.Col(2)*c(2,0);

    tipFrame = emFrame*Matrix33d::FromArmaMatrix3x3(iframe);
  }

  void EMNeedleTipMultiCalibrator::SaveSolution(const char *basePath, const char *extension)
  {
    for(std::map < s32, TipCalibData >::iterator i = m_calibs.begin(); i != m_calibs.end(); i++) {
      arma::mat total = i->second.tipFrame;
      total = arma::join_vert(total, i->second.c.t());
      if(i->first == 1)
        total.save(basePath+std::string(".")+extension, arma::raw_ascii);
      else
        total.save(basePath+std::to_string((_Longlong)i->first)+"."+extension, arma::raw_ascii);
    }

  }

  void EMNeedleTipMultiCalibrator::SetSolution(std::vector < std::string > paths, std::vector < s32 > frames)
  {
    for(s32 i=0; i<paths.size(); i++) {
      m_calibs[frames[i]] = TipCalibDataFromFilename(paths[i].c_str());
    }
  }

  bool EMNeedleTipMultiCalibrator::IsCalibrated()
  {
    return !m_calibs.empty();
  }

  void EMNeedleTipMultiCalibrator::ClearSolution()
  {
    ResetSolution();
  }

  void EMNeedleTipMultiCalibrator::ClearPoints()
  {
    ResetSolution();
    EMNeedleTipCalibrator::ResetSolution();
  }

  void EMNeedleTipMultiCalibrator::DoCalibration()
  {
    m_calibs.clear();
    for(s32 i=0; i<m_keyFrames.size(); i++) {
      vec c = solve(m_Rs[i],m_yxs[i]);

      mat tipFrame = eye<mat>(3,3);

      vec zaxis; zaxis << 1 << endr << 0 << endr << 0;
      vec yaxis = c; yaxis(0) = 0;
      yaxis = yaxis/norm(yaxis);
      vec xaxis = cross(yaxis,zaxis);
      tipFrame.submat(span(0,2), span(0,0)) = xaxis;
      tipFrame.submat(span(0,2), span(1,1)) = yaxis;
      tipFrame.submat(span(0,2), span(2,2)) = zaxis;

      TipCalibData curr;
      curr.tipFrame = tipFrame;
      curr.c = c;

      m_calibs[m_keyFrames[i]] = curr;
    }
  }

  void EMNeedleTipMultiCalibrator::AddPoint(const Vec3d &emPoint, const Matrix33d &emTipFrame, const Vec3d &tipPoint, s32 frame)
  {
    std::vector < s32 >::iterator it = std::find(m_keyFrames.begin(), m_keyFrames.end(), frame);
    if(it != m_keyFrames.end()) {
      s32 idx = it-m_keyFrames.begin();
      m_Rs[idx] = join_vert(m_Rs[idx],emTipFrame.ToArmaMat());
      m_yxs[idx] = join_vert(m_yxs[idx], (tipPoint-emPoint).ToArmaVec());
    } else {
      m_Rs.push_back(emTipFrame.ToArmaMat());
      m_yxs.push_back((tipPoint-emPoint).ToArmaVec());
      if(m_keyFrames.empty())
        m_keyFrames.push_back(1);
      else
        m_keyFrames.push_back(frame);
    }
  }

  void EMNeedleTipMultiCalibrator::ResetSolution()
  {
    m_calibs.clear();
    m_Rs.clear();
    m_yxs.clear();
    m_keyFrames.clear();
  }
  ////////////////////////////////////////////////////////////////
  //// END EMNeedleTipMultiCalibrator
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