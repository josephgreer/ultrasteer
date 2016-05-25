#include "JacobianControl.h"

void sortArray(f64 *a, u8 *idx, s32 nel)
{
  s32 ii;
  f64 temp;
  u8 tempIdx;
  for (s32 i = 0; i < nel; i++) {
    ii = i;
    idx[ii] = i;
    while (ii > 0 && a[ii - 1] > a[ii]) {
      temp = a[ii - 1];
      tempIdx = idx[ii - 1];
      a[ii - 1] = a[ii];
      idx[ii - 1] = idx[ii];
      a[ii] = temp;
      idx[ii] = tempIdx;
      ii--;
    }
  }
}

///////////////////////////////////////////////////////
/// BEGIN JACOBIANCONTROL
///////////////////////////////////////////////////////
JacobianControl::JacobianControl()
{
  memset(&m_qs[0], 0, sizeof(m_qs[0])*N_TURN_ACT);
  memset(&m_J[0], 0, sizeof(m_J[0])*N_TURN_ACT);
}

const f64 * JacobianControl::Update(Vec2f64 dx)
{
  u8 inflateActuators[N_TURN_ACT]; u8 deflateActuators[N_TURN_ACT];
  u8 currAct0, currAct1;
  s8 signedAct0, signedAct1;
  s32 nInf; s32 nDef; nInf = nDef = 0;
  Matrix22f64 JJ;
  f64 thetas[2 * N_TURN_ACT];
  s8 actuatorIdxs[2 * N_TURN_ACT];
  f64 dists[2 * N_TURN_ACT];
  u8 qIdxs[2 * N_TURN_ACT];
  s32 filled;

  Vec2f64 dx_actual, dx_desired, dx_curr, dq;
  dx_desired = dx;

  while (dx.magnitude() > 1e-6) {
    f64 theta = fmod(dx.angle() + 2 * PI, 2 * PI);
    filled = 0; nDef = 0; nInf = 0;
    for (s32 i = 0; i < N_TURN_ACT; i++) {
      if (m_qs[i] > 0)
        deflateActuators[nDef++] = i;
      if (m_qs[i] < 1)
        inflateActuators[nInf++] = i;
    }
    for (s32 i = 0; i < nInf; i++) {
      thetas[filled] = fmod(m_J[inflateActuators[i]].angle()+2*PI, 2*PI);
      actuatorIdxs[filled++] = (inflateActuators[i]+1);
    }
    for (s32 i = 0; i < nDef; i++) {
      thetas[filled] = fmod(m_J[deflateActuators[i]].angle() + 3*PI, 2*PI);
      actuatorIdxs[filled++] = -(deflateActuators[i]+1);
    }
    for (s32 i = 0; i < filled; i++) {
      dists[i] = ABS(thetas[i] - theta);
      dists[i] = MIN(dists[i], 2 * PI - dists[i]);
    }

    sortArray(dists, qIdxs, filled);

    currAct0 = ABS(actuatorIdxs[qIdxs[0]])-1; currAct1 = ABS(actuatorIdxs[qIdxs[1]])-1;
    signedAct0 = SIGN(actuatorIdxs[qIdxs[0]]); signedAct1 = SIGN(actuatorIdxs[qIdxs[1]]);

    if ((signedAct0*m_J[currAct0].dot(dx) < 0 && signedAct1*m_J[currAct1].dot(dx) < 0) || (currAct0 == currAct1)) {
      break;
    }

    JJ = Matrix22f64::FromCols(m_J[currAct0], m_J[currAct1]);
    dq = JJ.Inverse()*dx;

    f64 scale = 1;
    if (m_qs[currAct0] + dq.x < 0)
      scale = MIN(-m_qs[currAct0] / dq.x, scale);
    if(m_qs[currAct1] + dq.y < 0)
      scale = MIN(-m_qs[currAct1] / dq.y, scale);
    if (m_qs[currAct0] + dq.x > 1)
      scale = MIN((1 - m_qs[currAct0]) / dq.x, scale);
    if (m_qs[currAct1] + dq.y > 1)
      scale = MIN((1 - m_qs[currAct1]) / dq.y, scale);

    dq = dq*scale;
    if (scale == 0)
      break;

    dx_curr = JJ*dq;
    dx_actual = dx_actual+dx_curr;
    dx = dx-dx_curr;
    
    m_qs[currAct0] += dq.x;
    m_qs[currAct1] += dq.y;
  }

  return &m_qs[0];
}

const f64 * JacobianControl::Update(Vec2f64 dx, const Vec2f64 *J)
{
  memcpy(&m_J[0], J, sizeof(m_J[0])*N_TURN_ACT);
  return Update(dx);
}
///////////////////////////////////////////////////////
/// END JACOBIANCONTROL
///////////////////////////////////////////////////////
