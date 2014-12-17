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
function measurement = generateRandomMeasurement(x, u, params)

end