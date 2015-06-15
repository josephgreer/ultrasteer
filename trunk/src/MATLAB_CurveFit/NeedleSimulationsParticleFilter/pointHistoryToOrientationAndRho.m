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
function [q, rho] = pointHistoryToOrientationAndRho(x, u, params)
    % fit circle to three most recent points
    circ = Circle3([x.pos{1} x.pos{2} x.pos{3}]);
    
    normal = circ.N;
    
    % calculate frame
    binormal = circ.c-x.pos{1};
    binormal = binormal/norm(binormal);
    tangent = cross(normal, binormal);
    tangent = tangent/norm(tangent);
    
    if(dot(tangent, x.pos{2}-x.pos{1}) > 0)
        tangent = -tangent;
        binormal = -binormal;
        normal = -normal;
    end
    q = RotationMatrixToQuat([normal binormal tangent]);
    rho = circ.rho;
end