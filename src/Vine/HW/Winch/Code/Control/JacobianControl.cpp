#include "JacobianControl.h"
#include <assert.h>

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

////////////////////////////////////////////////////////
/// BEGIN JACOBIANCONTROL
////////////////////////////////////////////////////////
JacobianControl::JacobianControl()
{
}

Vecf64<N_TURN_ACT> JacobianControl::Update(Vecf64<2> dx, const Matrixf64<2, N_TURN_ACT>&J)
{
  m_J = J;
  return Update(dx);
}
///////////////////////////////////////////////////////
/// END JACOBIANCONTROL
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/// BEGIN JACOBIANHEURISTICCONTROL
///////////////////////////////////////////////////////
JacobianHeuristicControl::JacobianHeuristicControl()
  : JacobianControl()
{
}

Vecf64<N_TURN_ACT> JacobianHeuristicControl::Update(Vecf64<2> dx)
{
  u8 inflateActuators[N_TURN_ACT]; u8 deflateActuators[N_TURN_ACT];
  u8 currAct0, currAct1;
  s8 signedAct0, signedAct1;
  s32 nInf; s32 nDef; nInf = nDef = 0;
  Matrixf64<2,2> JJ;
  f64 thetas[2 * N_TURN_ACT];
  s8 actuatorIdxs[2 * N_TURN_ACT];
  f64 dists[2 * N_TURN_ACT];
  u8 qIdxs[2 * N_TURN_ACT];
  s32 filled;

  Vecf64<2> dx_actual, dx_desired, dx_curr, dq;
  dx_desired = dx;

  while (dx.magnitude() > 1e-6) {
    f64 theta = fmod(dx.angle() + 2 * PI, 2 * PI);
    filled = 0; nDef = 0; nInf = 0;
    for (s32 i = 0; i < N_TURN_ACT; i++) {
      if (m_q(i) > 0)
        deflateActuators[nDef++] = i;
      if (m_q(i) < 1)
        inflateActuators[nInf++] = i;
    }
    for (s32 i = 0; i < nInf; i++) {
      thetas[filled] = fmod(m_J.Col(inflateActuators[i]).angle()+2*PI, 2*PI);
      actuatorIdxs[filled++] = (inflateActuators[i]+1);
    }
    for (s32 i = 0; i < nDef; i++) {
      thetas[filled] = fmod(m_J.Col(deflateActuators[i]).angle() + 3*PI, 2*PI);
      actuatorIdxs[filled++] = -(deflateActuators[i]+1);
    }
    for (s32 i = 0; i < filled; i++) {
      dists[i] = ABS(thetas[i] - theta);
      dists[i] = MIN(dists[i], 2 * PI - dists[i]);
    }

    sortArray(dists, qIdxs, filled);

    currAct0 = ABS(actuatorIdxs[qIdxs[0]])-1; currAct1 = ABS(actuatorIdxs[qIdxs[1]])-1;
    signedAct0 = SIGN(actuatorIdxs[qIdxs[0]]); signedAct1 = SIGN(actuatorIdxs[qIdxs[1]]);

    if ((signedAct0*m_J.Col(currAct0).dot(dx) < 0 && signedAct1*m_J.Col(currAct1).dot(dx) < 0) || (currAct0 == currAct1)) {
      break;
    }

    JJ = Matrixf64<2,2>::FromCols(m_J.Col(currAct0), m_J.Col(currAct1));
    dq = JJ.Inverse()*dx;

    f64 scale = 1;
    if (m_q(currAct0) + dq.x < 0)
      scale = MIN(-m_q(currAct0) / dq.x, scale);
    if(m_q(currAct1) + dq.y < 0)
      scale = MIN(-m_q(currAct1) / dq.y, scale);
    if (m_q(currAct0) + dq.x > 1)
      scale = MIN((1 - m_q(currAct0)) / dq.x, scale);
    if (m_q(currAct1) + dq.y > 1)
      scale = MIN((1 - m_q(currAct1)) / dq.y, scale);

    dq = dq*scale;
    if (scale == 0)
      break;

    dx_curr = JJ*dq;
    dx_actual = dx_actual+dx_curr;
    dx = dx-dx_curr;
    
    m_q(currAct0) += dq.x;
    m_q(currAct1) += dq.y;
  }

  return m_q;
}
///////////////////////////////////////////////////////
/// END JACOBIANHEURISTICCONTROL
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
/// BEGIN JACOBIANBOXCONSTRAINTCONTROL
///////////////////////////////////////////////////////
template < unsigned char N > 
class IndexSet
{
public:
  s8 m_idxs[N];
  s8 m_nel;

  IndexSet()
    : m_nel(0)
  {
    Cleanup();
  }

  IndexSet(s8 *idxs, s8 nel)
  {
    memcpy(m_idxs, idxs, sizeof(s8)*nel);
    m_nel = nel;
    Cleanup();
  }

  //Take union of two mutually exclusive sets
  static IndexSet<N> MutuallyExclusiveUnion(const IndexSet &a, const IndexSet &b)
  {
    s8 idxs[N] = { 0 };
    s32 nel = 0;
    for (s32 i = 0; i < a.m_nel; i++)
      idxs[nel++] = a.m_idxs[i];
    for (s32 i = 0; i < b.m_nel; i++)
      idxs[nel++] = b.m_idxs[i];

    return IndexSet<N>(idxs, nel);
  }

  bool IsMember(u8 idx)
  {
    bool rv = false;
    for (s32 i = 0; i < m_nel; i++)
      rv = rv || (m_idxs[i] == idx);
    return rv;
  }

  void Cleanup()
  {
    for (s32 i = m_nel; i < N; i++)
      m_idxs[i] = -1;
  }

  void RemoveElement(u8 idx)
  {
    s8 wasFound = 0;
    for (s32 i = 0; i < m_nel; i++) {
      if (m_idxs[i] == idx) {
        wasFound = 1;
        m_nel--;
      }

      if (wasFound)
        m_idxs[i] = m_idxs[i + 1];
    }
    Cleanup();
  }

  void Clear()
  {
    m_nel = 0;
    Cleanup();
  }

  void AddElement(u8 idx)
  {
    if (m_nel >= N)
      return;

    m_idxs[m_nel++] = idx;
  }

  template < unsigned char O >
  void AssignElements(Vecf64 < N > &vectorToAssign, const Vecf64 < O > &vectorOfValues)
  {
    for (s32 i = 0; i < m_nel; i++)
      vectorToAssign(m_idxs[i]) = vectorOfValues(i);
  }

  s8 &operator()(s32 idx) { return m_idxs[idx]; }
};

template < unsigned char N, unsigned char O > 
static bool steps7Through11(Vecf64 <O> z, Vecf64 <N> &x, IndexSet <N> &U, IndexSet <N> &L, IndexSet <N> &F, const Vecf64 <N> &l, const Vecf64 <N> &u)
{
  bool doSteps2Through5 = false;
  f64 alpha = 1e10;
  for (s32 i = 0; i < O; i++) {
    if (z(i) <= l(F(i)) + EPS)
      alpha = MIN(alpha, ABS((u(F(i)) - x(F(i))) / (z(i) - x(F(i)))));
    else if (z(i) >= u(F(i)) - EPS)
      alpha = MIN(alpha, ABS((l(F(i)) - x(F(i))) / (z(i) - x(F(i)))));
  }
  if (alpha < 1) {
    for (s32 i = 0; i < O; i++)
      x(F(i)) = x(F(i)) + alpha*(z(i) - x(F(i)));
    doSteps2Through5 = false;


    U.Clear();
    L.Clear();
    F.Clear();

    for (s32 i = 0; i < N; i++) {
      if (x(i) > u(i) - EPS)
        U.AddElement(i);
      else if (x(i) < l(i) + EPS)
        L.AddElement(i);
      else
        F.AddElement(i);
    }
  }
  else {
    for (s32 i = 0; i < O; i++)
      x(F(i)) = z(F(i));
    doSteps2Through5 = true;
  }

  return doSteps2Through5;
}

template < unsigned char M, unsigned char N >
static Vecf64 < N > BVLS(const Matrixf64<M, N> &A, const Vecf64 <M> &b, const Vecf64<N> &l, const Vecf64<N> &u)
{
  enum COORDINATE_SET_MEMBER
  {
    CSM_U = 0,
    CSM_L = 1,
    CSM_F = 2
  };

  IndexSet<N> U, L, F, B;
  COORDINATE_SET_MEMBER member[N];

  IndexSet<N> *removeList;

  s8 maxIdx; f64 maxVal; 
  bool validIdx;

  Vecf64 < N > x;
  Vecf64 < M > bprime;

  Matrixf64 < M, M-1 > Aprime1;
  Matrixf64 < M, M > Aprime2;
  Vecf64 < M - 1 > z1;
  Vecf64 < M > z2;

  f64 *ww;

  s8 tempList[N] = { 0 };
  for (s32 i = 0; i < N; i++) {
    tempList[i] = i;
    member[i] = CSM_L;
  }
  L = IndexSet<N>(tempList, N);

  x = l;

  bool doSteps2Through5 = true;
  bool done = false;

  while (!done) {
    if (doSteps2Through5) {
      ww = A.Transpose()*(b - A*x);

      maxVal = -1e8;
      maxIdx = -1;
      for (s32 i = 0; i < N; i++) {
        validIdx = false;
        if (ww[i] > EPS && member[i] == CSM_L)
          validIdx = true;
        else if (ww[i] < -EPS && member[i] == CSM_U)
          validIdx = true;

        if (validIdx && ABS(ww[i]) > maxVal) {
          maxIdx = i;
          maxVal = ABS(ww[i]);
        }
      }

      // stopping criterion
      if (maxVal < 0)
        break;

      // remove elemnt from list
      if (member[maxIdx] == CSM_U)
        U.RemoveElement(maxIdx);
      else
        L.RemoveElement(maxIdx);

      member[maxIdx] = CSM_F;
      F.AddElement(maxIdx);
    }

    // Bounded variable set
    B = IndexSet<N>::MutuallyExclusiveUnion(L, U);
    bprime = b;
    for (s32 ii = 0; ii < B.m_nel; ii++)
      bprime = b - A.Col(B.m_idxs[ii])*x(B.m_idxs[ii]);

    if (F.m_nel == M - 1) {
      z1 = (Aprime1.Transpose()*Aprime1).Inverse()*Aprime1.Transpose()*bprime;
      doSteps2Through5 = steps7Through11(z1, x, U, L, F, l, u);
    }
    else if (F.m_nel == M) {
      z2 = (Aprime2.Inverse())*bprime;
      doSteps2Through5 = steps7Through11(z2, x, U, L, F, l, u);
    }
    else {
      assert(0);
    }
  }
  return x;
}

JacobianBoxConstraintControl::JacobianBoxConstraintControl()
  : JacobianControl()
{
}


Vecf64<N_TURN_ACT> JacobianBoxConstraintControl::Update(Vecf64<2> dx)
{
  Vecf64 <N_TURN_ACT> dq = BVLS<2, N_TURN_ACT>(m_J, dx, m_q*(-1.0), m_q*(-1.0) + 1.0);
  m_q = m_q + dq;
  return m_q;
}

///////////////////////////////////////////////////////
/// END JACOBIANBOXCONSTRAINTCONTROL
///////////////////////////////////////////////////////