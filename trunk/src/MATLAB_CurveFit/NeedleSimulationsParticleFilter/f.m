%% state update dynamics
% x = needle tip statee state
%   x.pos % position of needle tip frame 
%   x.q  % orientation of needle tip frame 
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
function xk1 = f(x,u, noiseParams, params)
%orienation noise angles
noiseTheta = mvnrnd(noiseParams.muOrientation, noiseParams.sigmaOrientation)';
noisePos = mvnrnd(noiseParams.muPos, noiseParams.sigmaPos)';
noiseVel = mvnrnd(noiseParams.muVelocity, noiseParams.sigmaVelocity);

x.q = quatmult(x.q,AxisAngleToQuat(noiseTheta));

u.v = u.v+noiseVel;
xk1 = propagateNeedleTip(x, u, params);

% add noise to x0, R, and rho
xk1.pos = xk1.pos+noisePos;
xk1.rho = max(xk1.rho+mvnrnd(noiseParams.muRho, noiseParams.sigmaRho),10);
xk1.w = x.w;
end