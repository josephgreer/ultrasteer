% intialize particles based on true state of needle 
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function xp = initializeParticles(x, params)
xp = {};
% for each particle
posNoise = mvnrnd(params.initPosMu,params.initPosSigma,params.np);
pos = repmat(x.pos',params.np,1);
pos = pos+posNoise;

rhoNoise = mvnrnd(params.initRhoMu, params.initRhoSigma, params.np);
rho = repmat(x.rho, params.np, 1);
rho = rho+rhoNoise;

orientationNoise = mvnrnd(params.initOrientationMu, params.initOrientationSigma, params.np);

for p=1:params.np
    xp{p}.pos = pos(p,:)';
    xp{p}.q = quatmult(x.q,AxisAngleToQuat(orientationNoise(p,:)'));
    xp{p}.rho = rho(p);
end

xp = xp';

end