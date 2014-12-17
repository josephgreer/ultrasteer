%% state update dynamics
% x = needle tip statee state
%   x.pos % position of needle tip frame n timesteps back
%   x.R  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature
% u = control input
%   u{1}        = current action
%   u{2}        = action 1 timestamp back
%   ...
%   u{n}        = action n timestamps back
%   u{i}.v         = insertion velocity
%   u{i}.dtheta    = rotation about needle's axis
%   u{i}.dc        = duty cycle ratio
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function xk1 = f(x,u,params)
%orienation noise angles
noiseTheta = mvnrnd(params.muOrientation, params.sigmaOrientation)';
noisePos = mvnrnd(params.muPos, params.sigmaPos)';

x.R = x.R*Rz(noiseTheta(3))*Ry(noiseTheta(2))*Rx(noiseTheta(1));
xk1 = propagateNeedleTip(x, u, params);

% add noise to x0, R, and rho
xk1.pos = xk1.pos+noisePos;
%xk1.R = Rz(noiseTheta(3))*Ry(noiseTheta(2))*Rx(noiseTheta(1))*xk1.R;
xk1.rho = max(xk1.rho+mvnrnd(params.muRho, params.sigmaRho),10);
end