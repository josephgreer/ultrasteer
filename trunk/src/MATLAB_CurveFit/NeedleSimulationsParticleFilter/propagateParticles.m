% propagate particles based on true state of needle 
% calls correct propagateParticles routine based on particle
% filter method.
% see ../NeedleSimulation.m for description of parameters
function x = propagateParticles(xp, u, params)
if(params.particleFilterMethod == 1)
    x = propagateParticles1(xp,u,params);
elseif(params.particleFilterMethod == 2)
    x = propagateParticles2(xp,u,params);
else
    x = propagateParticles100(xp, u, params);
end
end

%% state update dynamics
% x = needle tip statee state
%   x.pos % position of needle tip frame 
%   x.q  % orientation of needle tip frame 
%   x.rho radius of curvature
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
function x = propagateParticles1(xp, u, params)
x = {};
for i=1:length(xp)
    x{i} = f(xp{i},u,params);
end
x = x';
end

%% state update dynamics
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
function x = propagateParticles2(xp, u, params)
x = {};
for i=1:length(xp)
    % fit circle to three most recent points
    circ = Circle3([xp{i}.pos{1} xp{i}.pos{2} xp{i}.pos{3}]);
    % calculate frame
    binormal = circ.c-xp{i}.pos{1};
    binormal = binormal/norm(binormal);
    tangent = cross(circ.N, binormal);
    tangent = tangent/norm(tangent);
    
    xcurr = xp{i}.pos;
    xcurr.q = RotationMatrixToQuat([normal binormal tangent]);
    xcurr.rho = circ.r;
    xcurr.w = xp{i}.w;
    
    xcurr = f(xcurr,u,params);
    x{i}.pos = vertcat(xcurr.pos,xp{i}.pos);;
    % remove excess points if there are too many.
    x{i}.pos(params.n+1:end) = [];
    
    x{1}.w = xp{i}.w;
end
x = x';
end

%% state update dynamics
% x = needle tip state
%   x.dist = Gaussian representing current state of kalman filter
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
function x = propagateParticles100(xp, u, params)
% fake a pos since we don't care
xcurr.pos = zeros(3,1);
xcurr.q = RotationMatrixToQuat(xp{1}.dist.mu);
xcurr.rho = xp{1}.rho;

xc1 = propagateNeedleTip(xcurr, u, params);

R10 = QuatToRotationMatrix(xc1.q);

% prior orientation
R0 = QuatToRotationMatrix(xcurr.q);
%delta orientation
R1 = R10*R0';

% process noise
sigma1 = params.sigmaOrientation;
% prior uncertainty
sigma0 = xp{1}.dist.sigma;

x{1}.dist = SO3Gaussian(R10, sigma1+R1*sigma0*R1');
x{1}.rho = xp{1}.rho;
x{1}.w = 1;
end