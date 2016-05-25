#ifndef JACOBIANCONTROL_H
#define JACOBIANCONTROL_H

#include "Common.h"

class JacobianControl 
{
protected:
  f64 m_qs[N_TURN_ACT];
  Vec2f64 m_J[N_TURN_ACT];

public:
  JacobianControl();

  // Returns actuator q values to achieve end-effector displacement dx
  const f64 * Update(Vec2f64 dx);

  // Returns actuator q values to achieve end-effector displacement dx
  const f64 * Update(Vec2f64 dx, const Vec2f64 *J);

  void SetQs(f64 *q) { memcpy(&m_qs[0], q, sizeof(f64)*N_TURN_ACT); }
};

#endif
