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

public:
  DeadbandMapper(f64 deadband)
    : m_deadband(deadband)
  {
  }

  f64 MapVal(f64 x)
  {
    if (x == 0)
      return 0;

    return 255.0*((1 - m_deadband)*x + m_deadband);
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

class JacobianEstimator
{
public:
  virtual Matrixf64<2,N_TURN_ACT> Update(const Vecf64<2> &z) = 0;
  virtual Matrixf64<2, N_TURN_ACT> Update(const Vecf64<2> &z, const Vecf64<N_TURN_ACT> &dq) = 0;
  virtual Vecf64<N_TURN_ACT + 1> UpdateState(const Vecf64<2> &z) = 0;
  virtual Vecf64<N_TURN_ACT + 1> UpdateState(const Vecf64<2> &z, const Vecf64<N_TURN_ACT> &dq) = 0;
  virtual void Initialize(const Matrixf64<2, N_TURN_ACT> &J) = 0;
  virtual void Initialize(const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths) = 0;
  virtual void Initialize(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths) = 0;
  virtual void Initialize(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &R, const Matrixf64<2, 2> &Q, const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths) = 0;
  virtual void SetDq(const Vecf64<N_TURN_ACT> &dq) = 0;
  virtual Vecf64<N_TURN_ACT+1> GetState() = 0;
};

class EKFJacobianEstimator
{
protected:
  Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> m_R;
  Matrixf64<2, 2> m_Q;
  Vecf64<N_TURN_ACT> m_deltaTheta;
  Vecf64<N_TURN_ACT + 1> m_x;
  Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> m_E;
  Vecf64<N_TURN_ACT> m_dqLast;

public:
  EKFJacobianEstimator();
  EKFJacobianEstimator(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);
  EKFJacobianEstimator(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &R, const Matrixf64<2, 2> &Q, const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);

  virtual void Initialize(const Matrixf64<2, N_TURN_ACT> &J);
  virtual void Initialize(const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);
  virtual void Initialize(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);
  virtual void Initialize(const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &R, const Matrixf64<2, 2> &Q, const Matrixf64<N_TURN_ACT + 1, N_TURN_ACT + 1> &E, const Vecf64<N_TURN_ACT> &thetas, const Vecf64<N_TURN_ACT> strengths);

  virtual void SetDq(const Vecf64<N_TURN_ACT> &dq) { m_dqLast = dq; }
  virtual Matrixf64<2, N_TURN_ACT> Update(const Vecf64<2> &z);
  virtual Vecf64<N_TURN_ACT + 1> UpdateState(const Vecf64<2> &z);
  virtual Matrixf64<2, N_TURN_ACT> Update(const Vecf64<2> &z, const Vecf64<N_TURN_ACT> &dq);
  virtual Vecf64<N_TURN_ACT+1> UpdateState(const Vecf64<2> &z, const Vecf64<N_TURN_ACT> &dq);
  Vecf64<N_TURN_ACT+1> GetState() { return m_x; }

  void PrintState();
};

#endif
