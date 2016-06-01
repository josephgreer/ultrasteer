% Produced by CVXGEN, 2016-05-26 18:24:54 -0400.
% CVXGEN is Copyright (C) 2006-2012 Jacob Mattingley, jem@cvxgen.com.
% The code in this file is Copyright (C) 2006-2012 Jacob Mattingley.
% CVXGEN, or solvers produced by CVXGEN, cannot be used for commercial
% applications without prior written permission from Jacob Mattingley.

% Filename: cvxsolve.m.
% Description: Solution file, via cvx, for use with sample.m.
function [vars, status] = cvxsolve(params, settings)
J = params.J;
dx = params.dx;
q = params.q;
cvx_begin quiet
  % Caution: automatically generated by cvxgen. May be incorrect.
  variable dq(3, 1);

  minimize(quad_form(J*dq - dx, eye(2)));
  subject to
    0 <= q + dq;
    q + dq <= 1;
cvx_end
vars.dq = dq;
status.cvx_status = cvx_status;
% Provide a drop-in replacement for csolve.
status.optval = cvx_optval;
status.converged = strcmp(cvx_status, 'Solved');
