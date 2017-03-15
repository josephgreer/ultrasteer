#include "JacobianControl.h"
#ifdef __AVR_ATmega2560__
#include "Arduino.h"
#endif
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
  m_alpha = TIME_STEP / (TIME_STEP + 0.1);
}

void JacobianControl::Reset()
{
  for (s32 i = 0; i < N_TURN_ACT; i++) {
    m_q(i) = 0;
    m_lq(i) = 0;
    m_ldq(i) = 0;
  }
}

void JacobianControl::Update(Vecf64<N_TURN_ACT> &qs, Vecf64<N_TURN_ACT> &dqs, Vecf64<2> dx, const Matrixf64<2, N_TURN_ACT>&J)
{
  m_J = J;
  Update(qs, dqs, dx);
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

void JacobianHeuristicControl::Update(Vecf64<N_TURN_ACT> &qs, Vecf64<N_TURN_ACT> &dqs, Vecf64<2> dx)
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

  for (s32 i = 0; i < N_TURN_ACT; i++)
    dqs(i) = 0;

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
    dqs(currAct0) += dq.x;
    dqs(currAct1) += dq.y;
  }

  qs = m_q;
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
static bool steps7Through11(Vecf64 <O> z, Vecf64 <N> &x, IndexSet <N> &UpperBound, IndexSet <N> &LowerBound, IndexSet <N> &Free, const Vecf64 <N> &l, const Vecf64 <N> &u)
{
  bool doSteps2Through5 = false;
  f64 alpha = 1e10;
  for (s32 i = 0; i < O; i++) {
    if (z(i) <= l(Free(i)) + EPS)
      alpha = MIN(alpha, ABS((l(Free(i)) - x(Free(i))) / (z(i) - x(Free(i)))));
    else if (z(i) >= u(Free(i)) - EPS)
      alpha = MIN(alpha, ABS((u(Free(i)) - x(Free(i))) / (z(i) - x(Free(i)))));
  }
  if (alpha <= 1) {
    for (s32 i = 0; i < O; i++)
      x(Free(i)) = x(Free(i)) + alpha*(z(i) - x(Free(i)));
    doSteps2Through5 = false;


    UpperBound.Clear();
    LowerBound.Clear();
    Free.Clear();

    for (s32 i = 0; i < N; i++) {
      if (x(i) > u(i) - EPS)
        UpperBound.AddElement(i);
      else if (x(i) < l(i) + EPS)
        LowerBound.AddElement(i);
      else
        Free.AddElement(i);
    }
  }
  else {
    for (s32 i = 0; i < O; i++)
      x(Free(i)) = z(i);
    doSteps2Through5 = true;
  }

  return doSteps2Through5;
}

//#define PRINTF_DEBUG

template < unsigned char M, unsigned char N >
static Vecf64 < N > BVLS(const Matrixf64<M, N> &A, const Vecf64 <M> &b, const Vecf64<N> &l, const Vecf64<N> &u)
{
  enum COORDINATE_SET_MEMBER
  {
    CSM_U = 0,
    CSM_L = 1,
    CSM_F = 2
  };

  IndexSet<N> UpperBound, LowerBound, Free, Bound;
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
  Vecf64 < N > z3;

  f64 *ww;
  Vecf64 <N> w;

  s8 tempList[N] = { 0 };
  for (s32 i = 0; i < N; i++) {
    tempList[i] = i;
    member[i] = CSM_L;
  }
  LowerBound = IndexSet<N>(tempList, N);

  x = l;

  bool doSteps2Through5 = true;
  bool done = false;

  while (!done) {
#ifdef PRINTF_DEBUG
    Serial.println("x " + String(x(0), 10) + " " + String(x(1), 10) + " " + String(x(2), 10));
    delay(5);
#endif
    if (doSteps2Through5) {
      w = A.Transpose()*(b - A*x);
      ww = w;

#ifdef PRINTF_DEBUG
      Serial.println("ww " + String(ww[0],10) + " " + String(ww[1],10) + " " + String(ww[2],10));
      delay(5);
      Serial.println("Lower " + String((s32)LowerBound.m_idxs[0]) + " " + String((s32)LowerBound.m_idxs[1]) + " " + String((s32)LowerBound.m_idxs[2]));
      delay(5);
      Serial.println("Upper " + String((s32)UpperBound.m_idxs[0]) + " " + String((s32)UpperBound.m_idxs[1]) + " " + String((s32)UpperBound.m_idxs[2]));
      delay(5);
      Serial.println("Free " + String((s32)Free.m_idxs[0]) + " " + String((s32)Free.m_idxs[1]) + " " + String((s32)Free.m_idxs[2]));
      delay(5);
#endif

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

#ifdef PRINTF_DEBUG
      Serial.println("maxIdx " + String((s32)maxIdx) + " magnitude w " + String(w.magnitude(),10));
      delay(5);
#endif
      if (maxVal < 0 || w.magnitude() < 0.0005) {
#ifdef PRINTF_DEBUG
        Serial.println("finished");
        delay(5);
#endif
        break;
      }

      // remove elemnt from list
      if (member[maxIdx] == CSM_U)
        UpperBound.RemoveElement(maxIdx);
      else
        LowerBound.RemoveElement(maxIdx);

      member[maxIdx] = CSM_F;
      Free.AddElement(maxIdx);
    }

    // Bounded variable set
    Bound = IndexSet<N>::MutuallyExclusiveUnion(LowerBound, UpperBound);
    bprime = b;
    for (s32 ii = 0; ii < Bound.m_nel; ii++)
      bprime = bprime - A.Col(Bound.m_idxs[ii])*x(Bound.m_idxs[ii]);

#ifdef PRINTF_DEBUG
    Serial.println("ww " + String(ww[0], 10) + " " + String(ww[1], 10) + " " + String(ww[2], 10));
    delay(5);
    Serial.println("bound.nel " + String((s32)Bound.m_nel) + " bprime " + String(bprime(0), 10) + " " + String(bprime(1), 10));
    delay(5);
    Serial.println("Here I am");
#endif

    if (Free.m_nel == M - 1) {
      for (s32 ii = 0; ii < M - 1; ii++)
        Aprime1.SetCol(ii, A.Col(Free.m_idxs[ii]));
      z1 = (Aprime1.Transpose()*Aprime1).Inverse()*Aprime1.Transpose()*bprime;
#ifdef PRINTF_DEBUG
      Serial.println("z1 " + String(z1(0), 10));
      delay(5);
#endif
      doSteps2Through5 = steps7Through11(z1, x, UpperBound, LowerBound, Free, l, u);
    }
    else if (Free.m_nel == M) {
      for (s32 ii = 0; ii < M; ii++)
        Aprime2.SetCol(ii, A.Col(Free.m_idxs[ii]));
      z2 = (Aprime2.Inverse())*bprime;
#ifdef PRINTF_DEBUG
      Serial.println("z2 " + String(z2(0), 10) + " " + String(z2(1),10));
      delay(5);
#endif
      doSteps2Through5 = steps7Through11(z2, x, UpperBound, LowerBound, Free, l, u);
    }
    else if (Free.m_nel == 0) {
      doSteps2Through5 = true;
    }
    else if (Free.m_nel == N) {
#ifdef __AVR_ATmega2560__
      //Serial.println("All coordinates should not be in the free set.");
      //delay(10000000);
#endif
      break;
    } 
    else {
#ifdef __AVR_ATmega2560__
      Serial.println("Unexpected situation occurred.");
      delay(10000000);
#endif
    }
  }
  return x;
}

JacobianBoxConstraintControl::JacobianBoxConstraintControl()
  : JacobianControl()
{
}

Vecf64<3> runBVLS(Matrixf64<2, 3> J, Vecf64<3> l, Vecf64<3> u, Vecf64<2> dx)
{
  return BVLS<2, 3>(J, dx, l, u);
}


#define DO_PRINT_CONTROL

#ifdef DO_PRINT_CONTROL
s32 g_count = 0;
#endif
void JacobianBoxConstraintControl::Update(Vecf64<N_TURN_ACT> &qs, Vecf64<N_TURN_ACT> &dqs, Vecf64<2> dx)
{
  Vecf64 <N_TURN_ACT> dq = BVLS<2, N_TURN_ACT>(m_J, dx, m_q*(-1.0), m_q*(-1.0) + MAX_ACTUATOR_PRESSURE);

#if 0
  if (dq.magnitude() < EPS && dx.magnitude() > EPS) {
    Vecf64<N_TURN_ACT> temp = m_q*(-1.0);
    Serial.println("Fail");
    Serial.println("m_q = " + m_q.ToString());
    Serial.println("dx = " + dx.ToString());
    Serial.println("l = " + temp.ToString());
    temp = m_q*(-1.0)+1.0;
    Serial.println("u = " + temp.ToString());
    Serial.println("dq = " + dq.ToString());
    m_J.Print("JJ");
  }
#endif

  m_q = m_q + dq;
  qs = m_q;
  dqs = dq;

  m_lq = m_lq*(1.0 - m_alpha) + m_q*m_alpha;
  m_ldq = m_ldq*(1.0 - m_alpha) + dq*m_alpha;

#ifdef DO_PRINT_CONTROL
  if (++g_count == 100) {
    g_count = 0;
#ifdef __AVR_ATmega2560__
    Serial.println("q = [" + String(m_q(0), 6) + ", " + String(m_q(1), 6) + ", " + String(m_q(2), 6) + "] dq = [" + String(dq(0), 6) + ", " + String(dq(1), 6) + ", " + String(dq(2), 6) + "] dx = [" + String(dx(0), 6) + ", " + String(dx(1),6) + "]");
#endif
  }
#endif
}

///////////////////////////////////////////////////////
/// END JACOBIANBOXCONSTRAINTCONTROL
///////////////////////////////////////////////////////

Matrixf64<2, N_TURN_ACT> formJacobian(Vecf64<N_TURN_ACT + 1> x, Vecf64<N_TURN_ACT> &deltaThetas)
{
  Matrixf64<2, N_TURN_ACT> J;
  for (s32 i = 0; i < N_TURN_ACT; i++) {
    J(0, i) = x(i)*cos(x(N_TURN_ACT) + deltaThetas(i));
    J(1, i) = x(i)*sin(x(N_TURN_ACT) + deltaThetas(i));
  }
  return J;
}

///////////////////////////////////////////////////////
/// BEGIN EKFJacobianEstimator
///////////////////////////////////////////////////////
#define STRENGTH_VAR 400
#define THETA_VAR 0.01
#define MEASUREMENT_VAR 400
#define INIT_VAR 0.01

EKFJacobianEstimator::EKFJacobianEstimator()
{
  for (s32 i = 0; i < N_TURN_ACT; i++)
    m_dqLast(i) = -100;
}

EKFJacobianEstimator::EKFJacobianEstimator(const Matrixf64<N_TURN_ACT+1, N_TURN_ACT+1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths)
{
  Initialize(E, thetas, strengths);
}

EKFJacobianEstimator::EKFJacobianEstimator(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &R, const Matrixf64<2, 2> &Q, const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths)
{
  Initialize(R, Q, E, thetas, strengths);
}

void EKFJacobianEstimator::Initialize(const Matrixf64<2, N_TURN_ACT> &J)
{
  Vecf64<N_TURN_ACT> thetas;
  Vecf64<N_TURN_ACT> strengths;
  Vecf64<2> col;
  for (s32 i = 0; i < N_TURN_ACT; i++) {
    col = J.Col(i);
    thetas(i) = col.angle();
    strengths(i) = col.magnitude();
  }
  Initialize(thetas, strengths);
}

void EKFJacobianEstimator::Initialize(const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths)
{
  f64 E[4] = { INIT_VAR,INIT_VAR,INIT_VAR,INIT_VAR };
  Initialize(Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1>::Diagonal(E), thetas, strengths);
}

void EKFJacobianEstimator::Initialize(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths)
{
  f64 R[N_TURN_ACT + 1] = { 0 };
  for (s32 i = 0; i < N_TURN_ACT; i++)
    R[i] = STRENGTH_VAR;
  R[N_TURN_ACT] = THETA_VAR;

  f64 Q[2] = { MEASUREMENT_VAR,MEASUREMENT_VAR };
  Initialize(Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1>::Diagonal(R), Matrixf64<2, 2>::Diagonal(Q), E, thetas, strengths);
}

void EKFJacobianEstimator::Initialize(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &R, const Matrixf64<2, 2> &Q, const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths)
{
  m_R = R;
  m_Q = Q;
  m_E = E;
  for (s32 i = 0; i < N_TURN_ACT; i++)
    m_x(i) = strengths(i);
  m_x(N_TURN_ACT) = thetas(0);

  for (s32 i = 0; i < N_TURN_ACT; i++)
    m_deltaTheta(i) = thetas(i) - m_x(N_TURN_ACT);
  
  Serial.println("Init thetas = " + (thetas*(180.0 / PI)).ToString() + " deltaThetas = " + (m_deltaTheta*(180.0 / PI)).ToString());

  for (s32 i = 0; i < N_TURN_ACT; i++)
    m_dqLast(i) = -100;
}

Matrixf64<2, N_TURN_ACT> EKFJacobianEstimator::Update(const Vecf64<2> &z)
{
  if (m_dqLast(0) > -10)
    return Update(z, m_dqLast);

  return formJacobian(m_x, m_deltaTheta);
}

Vecf64<N_TURN_ACT + 1> EKFJacobianEstimator::UpdateState(const Vecf64<2> &z) 
{
  if (m_dqLast(0) > -10)
    return UpdateState(z, m_dqLast); 

  return m_x;
}

Matrixf64<2, N_TURN_ACT> EKFJacobianEstimator::UpdateDeadReckoning(const Vecf64<2> &z, s32 i)
{
  f64 theta = z.angle();
  Serial.println("Actuator = " + String(i) + " Z = " + z.ToString() + " theta = " + String(theta * 180 / PI));
  m_x(N_TURN_ACT) = theta - m_deltaTheta(i);
  return formJacobian(m_x, m_deltaTheta);
}

Matrixf64<2, N_TURN_ACT> EKFJacobianEstimator::Update(const Vecf64<2> &z, const Vecf64<N_TURN_ACT> &dq)
{
  UpdateState(z, dq);
  return formJacobian(m_x, m_deltaTheta);
}

void EKFJacobianEstimator::IncrementTheta(f64 dtheta)
{
  return;
}

Vecf64<N_TURN_ACT + 1> EKFJacobianEstimator::UpdateState(const Vecf64<2> &z, const Vecf64<N_TURN_ACT> &dq)
{
  Vecf64<N_TURN_ACT + 1> xtbar = m_x;
  Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> Etbar;
  Etbar = m_E + m_R;

  Matrixf64<2, N_TURN_ACT + 1> Ht;
  f64 cang, sang;
  for (s32 i = 0; i < N_TURN_ACT; i++) {
    cang = cos(xtbar(N_TURN_ACT) + m_deltaTheta(i));
    sang = sin(xtbar(N_TURN_ACT) + m_deltaTheta(i));

    Ht(0, i) = cang*dq(i);
    Ht(1, i) = sang*dq(i);
    Ht(0, N_TURN_ACT) += -sang*xtbar(i)*dq(i);
    Ht(1, N_TURN_ACT) += cang*xtbar(i)*dq(i);
  }

  Vecf64<2> hxtbar = formJacobian(xtbar, m_deltaTheta)*dq;
  Matrixf64<N_TURN_ACT + 1, 2> Kt = Etbar*Ht.Transpose()*((Ht*Etbar*Ht.Transpose() + m_Q).Inverse());
  m_x = xtbar + Kt*(z - hxtbar);
  m_x(N_TURN_ACT) = fmod(m_x(N_TURN_ACT), 2 * PI);
  m_E = (Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1>::Identity() - Kt*Ht)*Etbar;
  return m_x;
}
///////////////////////////////////////////////////////
/// END EKFJacobianEstimator
///////////////////////////////////////////////////////

Matrixf64<2, N_TURN_ACT> unflattenVector(const Vecf64<2 * N_TURN_ACT> &v)
{
  Matrixf64<2, N_TURN_ACT> res;
  for (s32 c = 0; c < N_TURN_ACT; c++) {
    res(0, c) = v(2 * c);
    res(1, c) = v(2 * c + 1);
  }
  return res;
}

Vecf64<2 * N_TURN_ACT> flattenMatrix(const Matrixf64<2, N_TURN_ACT> &m)
{
  Vecf64<2 * N_TURN_ACT> res;
  for (s32 i = 0; i < N_TURN_ACT; i++) {
    res(2 * i + 0) = m(0, i);
    res(2 * i + 1) = m(1, i);
  }
  return res;
}

///////////////////////////////////////////////////////
/// BEGIN KFJacobianEstimator
///////////////////////////////////////////////////////
#define KF_VAR 1
#define KF_MEASUREMENT_VAR 10
#define KF_INIT_VAR 3

KFJacobianEstimator::KFJacobianEstimator()
{
  m_alpha = 1.0;// TIME_STEP / (TIME_STEP + 0.1);
  for (s32 i = 0; i < 2 * N_TURN_ACT; i++)
    m_x(i) = 0;
}

void KFJacobianEstimator::Initialize(const Matrixf64<2, N_TURN_ACT> &J)
{
  f64 R[2 * N_TURN_ACT] = { 0 };
  f64 E[2 * N_TURN_ACT] = { 0 };
  for (s32 i = 0; i < 2 * N_TURN_ACT; i++) {
    R[i] = KF_VAR;
    E[i] = KF_INIT_VAR;
  }
  Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT> RR = Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT>::Diagonal(R);
  Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT> EE = Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT>::Diagonal(E);
  f64 Q[2] = { KF_MEASUREMENT_VAR,KF_MEASUREMENT_VAR };
  Matrixf64<2, 2> QQ = Matrixf64<2, 2>::Diagonal(Q);

  Initialize(J, RR, QQ, EE);
}

void KFJacobianEstimator::Initialize(const Matrixf64<2, N_TURN_ACT> &J, const Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT> &R, const Matrixf64<2, 2> &Q, const Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT> &E)
{
  m_x = flattenMatrix(J);

  m_Q = Q;
  m_R = R;
  m_E = E;
}

Matrixf64<2, N_TURN_ACT> KFJacobianEstimator::Update(const Vecf64<2> z)
{
  return Update(z, m_dq);
}

Matrixf64<2, N_TURN_ACT> KFJacobianEstimator::Update(const Vecf64<2> z, const Vecf64<N_TURN_ACT> dq)
{
  Vecf64<2 * N_TURN_ACT> xtbar = m_x;
  Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT> Etbar;
  Etbar = m_E + m_R;

  Matrixf64<2, 2 * N_TURN_ACT> Ct;
  for (s32 i = 0; i < N_TURN_ACT; i++) {
    Ct(0, 2 * i) = dq(i);
    Ct(1, 2 * i + 1) = dq(i);
  }

  Matrixf64<2 * N_TURN_ACT, 2> Kt = Etbar*Ct.Transpose()*((Ct*Etbar*Ct.Transpose() + m_Q).Inverse());
  m_x = xtbar + Kt*(z - Ct*xtbar);
  m_E = (Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT>::Identity() - Kt*Ct)*Etbar;

  return unflattenVector(m_x);
}

void KFJacobianEstimator::PrintState()
{
#ifdef __AVR_ATmega2560__
  Matrixf64<2, N_TURN_ACT> J = unflattenVector(m_x);
  J.Print("J");
#endif
}

Vecf64<N_TURN_ACT> KFJacobianEstimator::GetAngles()
{
  Vecf64<N_TURN_ACT> thetas;
  Matrixf64<2, N_TURN_ACT> JJ = unflattenVector(m_x);
  for (s32 i = 0; i < N_TURN_ACT; i++) {
    if (JJ.Col(i).magnitude() > 1e-3)
      thetas(i) = JJ.Col(i).angle() * 180.0 / PI;
    else
      thetas(i) = 0;
  }
  return thetas;
}

Vecf64<N_TURN_ACT> KFJacobianEstimator::GetStrengths()
{
  Vecf64<N_TURN_ACT> mags;
  Matrixf64<2, N_TURN_ACT> JJ = unflattenVector(m_x);
  for (s32 i = 0; i < N_TURN_ACT; i++) {
    mags(i) = JJ.Col(i).magnitude();
  }
  return mags;
}
///////////////////////////////////////////////////////
/// END KFJacobianEstimator
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/// BEGIN BroydenJacobianEstimator
///////////////////////////////////////////////////////
BroydenJacobianEstimator::BroydenJacobianEstimator()
  : m_nabla(0.5)
{
}

void BroydenJacobianEstimator::Initialize(const Matrixf64<2, N_TURN_ACT> &J)
{
  m_J = J;
}

Matrixf64<2, N_TURN_ACT> BroydenJacobianEstimator::Update(const Vecf64<2> z, const Vecf64<N_TURN_ACT> dq)
{
  Matrixf64 <1, N_TURN_ACT> dqt;
  for (s32 i = 0; i < N_TURN_ACT; i++)
    dqt(0,i) = dq(i);
  Vecf64<2> dzmjq = z - m_J*dq;
  Matrixf64<2, 1> ddd;
  for (s32 i = 0; i < 2; i++)
    ddd(i, 0) = dzmjq(i);
  m_J = m_J + (ddd*dqt) * (m_nabla / dq.magnitudeSquared());
  return m_J;
}

Vecf64<N_TURN_ACT> BroydenJacobianEstimator::GetAngles()
{
  Vecf64 <N_TURN_ACT> angles;
  for (s32 i = 0; i < N_TURN_ACT; i++)
    angles(i) = m_J.Col(i).angle() * 180.0 / PI;
  return angles;
}

Vecf64<N_TURN_ACT> BroydenJacobianEstimator::GetStrengths()
{
  Vecf64 <N_TURN_ACT> mags;
  for (s32 i = 0; i < N_TURN_ACT; i++)
    mags(i) = m_J.Col(i).magnitude();
  return mags;
}
///////////////////////////////////////////////////////
/// END BroydenJacobianEstimator
///////////////////////////////////////////////////////