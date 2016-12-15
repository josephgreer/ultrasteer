/* Produced by CVXGEN, 2016-05-26 18:24:55 -0400.  */
/* CVXGEN is Copyright (C) 2006-2012 Jacob Mattingley, jem@cvxgen.com. */
/* The code in this file is Copyright (C) 2006-2012 Jacob Mattingley. */
/* CVXGEN, or solvers produced by CVXGEN, cannot be used for commercial */
/* applications without prior written permission from Jacob Mattingley. */

/* Filename: matrix_support.c. */
/* Description: Support functions for matrix multiplication and vector filling. */
#include "solver.h"
#ifdef __AVR_ATmega2560__
#pragma GCC push_options
#pragma GCC optimize ("O3")
#endif
void multbymA(double *lhs, double *rhs) {
  lhs[0] = -rhs[0]*(params.J[0])-rhs[1]*(params.J[2])-rhs[2]*(params.J[4])-rhs[3]*(-1);
  lhs[1] = -rhs[0]*(params.J[1])-rhs[1]*(params.J[3])-rhs[2]*(params.J[5])-rhs[4]*(-1);
}
void multbymAT(double *lhs, double *rhs) {
  lhs[0] = -rhs[0]*(params.J[0])-rhs[1]*(params.J[1]);
  lhs[1] = -rhs[0]*(params.J[2])-rhs[1]*(params.J[3]);
  lhs[2] = -rhs[0]*(params.J[4])-rhs[1]*(params.J[5]);
  lhs[3] = -rhs[0]*(-1);
  lhs[4] = -rhs[1]*(-1);
}
void multbymG(double *lhs, double *rhs) {
  lhs[0] = -rhs[0]*(-1);
  lhs[1] = -rhs[1]*(-1);
  lhs[2] = -rhs[2]*(-1);
  lhs[3] = -rhs[0]*(1);
  lhs[4] = -rhs[1]*(1);
  lhs[5] = -rhs[2]*(1);
}
void multbymGT(double *lhs, double *rhs) {
  lhs[0] = -rhs[0]*(-1)-rhs[3]*(1);
  lhs[1] = -rhs[1]*(-1)-rhs[4]*(1);
  lhs[2] = -rhs[2]*(-1)-rhs[5]*(1);
  lhs[3] = 0;
  lhs[4] = 0;
}
void multbyP(double *lhs, double *rhs) {
  /* TODO use the fact that P is symmetric? */
  /* TODO check doubling / half factor etc. */
  lhs[0] = 0;
  lhs[1] = 0;
  lhs[2] = 0;
  lhs[3] = rhs[3]*(2);
  lhs[4] = rhs[4]*(2);
}
void fillq(void) {
  work.q[0] = 0;
  work.q[1] = 0;
  work.q[2] = 0;
  work.q[3] = 0;
  work.q[4] = 0;
}
void fillh(void) {
  work.h[0] = params.q[0];
  work.h[1] = params.q[1];
  work.h[2] = params.q[2];
  work.h[3] = -(-1+params.q[0]);
  work.h[4] = -(-1+params.q[1]);
  work.h[5] = -(-1+params.q[2]);
}
void fillb(void) {
  work.b[0] = params.dx[0];
  work.b[1] = params.dx[1];
}
void pre_ops(void) {
}
#ifdef __AVR_ATmega2560__
#pragma GCC pop_options
#endif
