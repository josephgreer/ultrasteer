% intialize particles based on true state of needle 
% calls correct initializeParticlesRoutine based on particle
% filter method.
% see ../NeedleSimulation.m for description of parameters
function xp = initializeParticles(x, u, params)
if(params.particleFilterMethod == 1)
    xp = initializeParticles1(x,u,params);
elseif(params.particleFilterMethod == 2)
    xp = initializeParticles2(x,u,params);
else
    xp = initializeParticles100(x,u,params);
end
end

% intialize particles based on true state of needle 
% x = needle tip state
% x{1} = current needle tip state
% x{2} = needle tip state 1 time step ago
% ...
%   x{i}.pos % position of needle tip frame i timesteps back
%   x{i}.q  % orientation of needle tip frame i timesteps back
%   x{i}.rho radius of curvature (mm) i timesteps back
%   x{i}.w = particle weight i timesteps back
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

x = x{1};

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
function xp = initializeParticles2(x, u, params)
xp = {};

pos = cell2mat(x);
pos = [pos.pos]';
pos = repmat(pos, params.np, 1);
posNoise = mvnrnd(params.p2.initPosMu, params.p2.initPosSigma,...
    params.np*params.n);
pos = pos+posNoise;

for p=1:params.np
    xp{p}.pos = mat2cell(pos((p-1)*params.n+1:p*params.n,:)', 3, ones(params.n,1));
    xp{p}.w = 1/params.np;
end

xp = xp';

end


% intialize particles based on true state of needle 
% this is for "100th" version of "particle filter"
% which is actually a kalman filter for rotation
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
function xp =initializeParticles100(x, u, params)
xp{1}.dist = SO3Gaussian(QuatToRotationMatrix(x{1}.q), params.p100.initOrientationSigma);
xp{1}.rho = x{1}.rho;
xp{1}.w = 1;
end