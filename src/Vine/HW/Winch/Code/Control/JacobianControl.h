#ifndef JACOBIANCONTROL_H
#define JACOBIANCONTROL_H

#include "Common.h"

class JacobianControl
{
protected:
  Vecf64<N_TURN_ACT> m_q;
  Matrixf64<2, N_TURN_ACT> m_J;

public:
  JacobianControl();

  // Returns actuator q values to achieve end-effector displacement dx
  virtual Vecf64<N_TURN_ACT> Update(Vecf64<2> dx) = 0;

  // Returns actuator q values to achieve end-effector displacement dx
  virtual Vecf64<N_TURN_ACT> Update(Vecf64<2> dx, const Matrixf64<2,N_TURN_ACT>&J);

  virtual void SetQs(const Vecf64<N_TURN_ACT> &q) { m_q = q; }
};

class JacobianHeuristicControl : public JacobianControl
{
public:
  JacobianHeuristicControl();

  // Returns actuator q values to achieve end-effector displacement dx
  virtual Vecf64<N_TURN_ACT> Update(Vecf64<2> dx);
};

class JacobianBoxConstraintControl : public JacobianControl
{
public:
  JacobianBoxConstraintControl();

  // Returns actuator q values to achieve end-effector displacement dx
  virtual Vecf64<N_TURN_ACT> Update(Vecf64<2> dx);
};

#endif
