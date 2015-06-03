% intialize particles based on true state of needle 
% this is for "2nd" version of particle filter 
% where a history of points is kept track of and 
% frames/curvatures are generated on the fly based on 
% previous point histroy
% x = needle tip state
%   x.pos{1} % position of needle tip frame
%   x.pos{2} % position of needle tip frame 1 step back
%   ...
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
function xp = initializeParticles2(x, params)
xp = {};

% for each particle
posNoise = mvnrnd(QuatToRotationMatrix(x.q)*params.p1.initPosMu,params.p1.initPosSigma,params.np);
pos = repmat(x.pos',params.np,1);
pos = pos+posNoise;

rhoNoise = mvnrnd(params.initRhoMu, params.initRhoSigma, params.np);
rho = repmat(x.rho, params.np, 1);
rho = rho+rhoNoise;

orientationNoise = mvnrnd(params.initOrientationMu, params.initOrientationSigma, params.np);

for p=1:params.np
    xcurr.pos = pos(p,:)';
    xcurr.q = quatmult(x.q,AxisAngleToQuat(orientationNoise(p,:)'));
    xcurr.rho = rho(p);
    xcurr.w = 1/params.np;
    
    xp{p}.pos = propagateNeedleBack(xcurr, u, params);
    xp{p}.w = 1/params.np;
end

xp = xp';

end