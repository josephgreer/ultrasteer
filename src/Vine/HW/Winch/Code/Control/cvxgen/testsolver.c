/* Produced by CVXGEN, 2016-05-26 18:24:55 -0400.  */
/* CVXGEN is Copyright (C) 2006-2012 Jacob Mattingley, jem@cvxgen.com. */
/* The code in this file is Copyright (C) 2006-2012 Jacob Mattingley. */
/* CVXGEN, or solvers produced by CVXGEN, cannot be used for commercial */
/* applications without prior written permission from Jacob Mattingley. */

/* Filename: testsolver.c. */
/* Description: Basic test harness for solver.c. */
#include "solver.h"
Vars vars;
Params params;
Workspace work;
Settings settings;
#define NUMTESTS 1
int main(int argc, char **argv) {
  int num_iters;
#if (NUMTESTS > 0)
  int i;
  double time;
  double time_per;
#endif
  time = 0;
  set_defaults();
  setup_indexing();
  load_default_data();
  /* Solve problem instance for the record. */
  settings.verbose = 1;
  num_iters = solve();
#ifndef ZERO_LIBRARY_MODE
#if (NUMTESTS > 0)
  /* Now solve multiple problem instances for timing purposes. */
  settings.verbose = 0;
  //tic();
  for (i = 0; i < NUMTESTS; i++) {
    solve();
  }
  //time = tocq();
  printf("Timed %d solves over %.3f seconds.\n", NUMTESTS, time);
  time_per = time / NUMTESTS;
  if (time_per > 1) {
    printf("Actual time taken per solve: %.3g s.\n", time_per);
  } else if (time_per > 1e-3) {
    printf("Actual time taken per solve: %.3g ms.\n", 1e3*time_per);
  } else {
    printf("Actual time taken per solve: %.3g us.\n", 1e6*time_per);
  }
#endif
#endif
  return 0;
}
void load_default_data(void) {
  params.J[0] = 0.20319161029830202;
  params.J[1] = 0.8325912904724193;
  params.J[2] = -0.8363810443482227;
  params.J[3] = 0.04331042079065206;
  params.J[4] = 1.5717878173906188;
  params.J[5] = 1.5851723557337523;
  params.dx[0] = -1.497658758144655;
  params.dx[1] = -1.171028487447253;
  params.q[0] = -1.7941311867966805;
  params.q[1] = -0.23676062539745413;
  params.q[2] = -1.8804951564857322;
}
