% intialize particles based on true state of needle 
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weight
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
function xp = initializeParticles1(x, u, params)
xp = {};

% for each particle
posNoise = mvnrnd(QuatToRotationMatrix(x.q)*params.p1.initPosMu,params.p1.initPosSigma,params.np);
pos = repmat(x.pos',params.np,1);
pos = pos+posNoise;

rhoNoise = mvnrnd(params.p1.initRhoMu, params.p1.initRhoSigma, params.np);
rho = repmat(x.rho, params.np, 1);
rho = rho+rhoNoise;

orientationNoise = mvnrnd(params.p1.initOrientationMu, params.p1.initOrientationSigma, params.np);

for p=1:params.np
    xp{p}.pos = pos(p,:)';
    xp{p}.q = quatmult(x.q,AxisAngleToQuat(orientationNoise(p,:)'));
    xp{p}.rho = rho(p);
    xp{p}.w = 1/params.np;
end

xp = xp';

end