#ifndef JACOBIANCONTROL_H
#define JACOBIANCONTROL_H

#include "Common.h"

class ActuatorMapper
{
public:
  virtual f64 MapVal(f64 x) = 0;
};

class DeadbandMapper : public ActuatorMapper
{
protected:
  f64 m_deadband;
  f64 m_maxPWM;

public:
  DeadbandMapper(f64 deadband, f64 maxPWM)
    : m_deadband(deadband)
    , m_maxPWM(maxPWM)
  {
  }

  f64 MapVal(f64 x)
  {
    if (x == 0)
      return 0;

    return (f64)(m_maxPWM)*((1 - m_deadband)*x + m_deadband);
  }
};

class SigmoidMapper : public ActuatorMapper
{
protected:
  f64 m_a, m_b, m_c, m_d;

public:
  SigmoidMapper(f64 a, f64 b, f64 c, f64 d)
    : m_a(a)
    , m_b(b)
    , m_c(c)
    , m_d(d)
  {
  }

  f64 MapVal(f64 x)
  {
    return CLAMP((s32)((m_d*m_c + log10(MAX(x - m_a, EPS)) - log10(MAX(m_b - x, EPS))) / m_d), 0, 255);
  }
};

class JacobianControl
{
protected:
  Vecf64<N_TURN_ACT> m_q;
  Matrixf64<2, N_TURN_ACT> m_J;
  f64 m_alpha;
  Vecf64<N_TURN_ACT> m_lq;
  Vecf64<N_TURN_ACT> m_ldq;

public:
  JacobianControl();

  // Returns actuator q values to achieve end-effector displacement dx
  virtual void Update(Vecf64<N_TURN_ACT> &qs, Vecf64<N_TURN_ACT> &dqs, Vecf64<2> dx) = 0;

  // Returns actuator q values to achieve end-effector displacement dx
  virtual void Update(Vecf64<N_TURN_ACT> &qs, Vecf64<N_TURN_ACT> &dqs, Vecf64<2> dx, const Matrixf64<2,N_TURN_ACT>&J);

  virtual void SetQs(const Vecf64<N_TURN_ACT> &q) { m_q = q; }
};

class JacobianHeuristicControl : public JacobianControl
{
public:
  JacobianHeuristicControl();

  // Returns actuator q values to achieve end-effector displacement dx
  virtual void Update(Vecf64<N_TURN_ACT> &qs, Vecf64<N_TURN_ACT> &dqs, Vecf64<2> dx);
};

class JacobianBoxConstraintControl : public JacobianControl
{
public:
  JacobianBoxConstraintControl();

  // Returns actuator q values to achieve end-effector displacement dx
  virtual void Update(Vecf64<N_TURN_ACT> &qs, Vecf64<N_TURN_ACT> &dqs, Vecf64<2> dx);
};

#define N_COMBINE 5

template < class T >
struct FixedQueue
{
  T m[N_COMBINE];
  s32 n;

  FixedQueue()
    : n(0)
  {
  }

  void AddElement(const T &el)
  {
    for (s32 i = N_COMBINE-1; i > 1; i--) {
      m[i] = m[i - 1];
    }
    m[0] = el;
    n = MIN(n+1,N_COMBINE);
  }

  T SumElements()
  {
    T sum;
    sum = 0;
    Serial.println("sum mag start " + String(sum.magnitude(), 6));
    Serial.println(sum.magnitude(),6);
    for (s32 i = 0; i < n; i++) {
      sum = sum + m[i];
    }
    Serial.println("sum mag end " + String(sum.magnitude(), 6));
    return sum;
  }

  void Clear()
  {
    n = 0;
  }
};

class EKFJacobianEstimator
{
protected:
  Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> m_R;
  Matrixf64<2, 2> m_Q;
  Vecf64<N_TURN_ACT> m_deltaTheta;
  Vecf64<N_TURN_ACT + 1> m_x;
  Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> m_E;
  Vecf64<N_TURN_ACT> m_dq;
  Vecf64<2> m_dz;
  f64 m_alpha;

public:
  EKFJacobianEstimator();
  EKFJacobianEstimator(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);
  EKFJacobianEstimator(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &R, const Matrixf64<2, 2> &Q, const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);

  virtual void Initialize(const Matrixf64<2, N_TURN_ACT> &J);
  virtual void Initialize(const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);
  virtual void Initialize(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);
  virtual void Initialize(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &R, const Matrixf64<2, 2> &Q, const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);

  virtual void SetDq(const Vecf64<N_TURN_ACT> &dq) 
  {
  	m_dq = m_dq*(1.0-m_alpha)+dq*m_alpha;
  }

  virtual void SetState(const Vecf64<N_TURN_ACT + 1> &x) { m_x = x; }
  
  virtual Matrixf64<2, N_TURN_ACT> Update(const Vecf64<2> &z);
  virtual Vecf64<N_TURN_ACT + 1> UpdateState(const Vecf64<2> &z);
  virtual Vecf64<N_TURN_ACT+1> UpdateState(const Vecf64<2> z, const Vecf64<N_TURN_ACT> dq);
  Vecf64<N_TURN_ACT+1> GetState() { return m_x; }

  void PrintState();
};

Matrixf64<2, N_TURN_ACT> unflattenVector(const Vecf64<2 * N_TURN_ACT> &v);
Vecf64<2 * N_TURN_ACT> flattenMatrix(const Matrixf64<2, N_TURN_ACT> &m);

class KFJacobianEstimator
{
protected:
  Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT> m_R;
  Matrixf64<2, 2> m_Q;
  Vecf64<2 * N_TURN_ACT> m_x;
  Matrixf64<2 * N_TURN_ACT, 2* N_TURN_ACT> m_E;
  Vecf64<N_TURN_ACT> m_dq;
  Vecf64<2> m_dz;
  f64 m_alpha;

public:
  KFJacobianEstimator();

  void SetAlpha(f64 alpha) { m_alpha = alpha; }

  virtual void SetJacobian(const Matrixf64<2, N_TURN_ACT>&J) { m_x = flattenMatrix(J); }

  virtual void Initialize(const Matrixf64<2, N_TURN_ACT> &J);
  virtual void Initialize(const Matrixf64<2, N_TURN_ACT> &J, const Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT> &R, const Matrixf64<2, 2> &Q, const Matrixf64<2 * N_TURN_ACT, 2 * N_TURN_ACT> &E);

  virtual void SetDq(const Vecf64<N_TURN_ACT> &dq)
  {
    m_dq = m_dq*(1.0 - m_alpha) + dq*m_alpha;
  }


  virtual Matrixf64<2, N_TURN_ACT> Update(const Vecf64<2> z);
  virtual Matrixf64<2, N_TURN_ACT> Update(const Vecf64<2> z, const Vecf64<N_TURN_ACT> dq);

  void PrintState();
};

#endif
