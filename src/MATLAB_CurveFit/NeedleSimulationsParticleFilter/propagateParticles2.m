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