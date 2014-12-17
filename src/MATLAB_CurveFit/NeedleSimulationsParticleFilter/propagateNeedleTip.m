% x = needle tip statee state
%   x.pos % position of needle tip frame n timesteps back
%   x.R  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
% u = control input
%   u.v         = insertion velocity (mm/s)
%   u.dtheta    = rotation about needle's axis
%   u.dc        = duty cycle ratio
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function x1 = propagateNeedleTip(x, u, params)
dl = u.v*params.dt;


% Rk' coordinate system expressed in global coordinates.
% note rotation order is flipped because using intrinsic rotations with
% extrinisc rotation matrices
Rkp = x.R*Rz(u.dtheta);

% location of tip expressed in Rk' coordinates
kp_x = [0; x.rho*(1-cos(dl/x.rho)); x.rho*sin(dl/x.rho)];

% convert needle tip location into global coordinates
x1.pos = Rkp*kp_x+x.pos;
% pass through rho
x1.rho = x.rho;

% new coordinate frame expressed in global coordinates.
% new coordinate frame is old coordinate frame rhoated by u.dtheta around
% needle tip's z-axis then rotated about the needle tip's x axis due to
% following the curved arc.  Note taht rotation order is flipped due to
% intrinsic axis rotations used rather than extrinsic.
x1.R = Rkp*Rx(-dl/x.rho);
end