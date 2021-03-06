% x = needle tip statee state
%   x.x0 % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weight

% u{1} = control input that lead to xs(1)
% u{2} = control input that lead to xs(2)
% ...
% u{n} = control input that lead to xs{n}

% u{i} = control input
%   u{i}.v         = insertion velocity (mm/s)
%   u{i}.dtheta    = rotation about needle's axis
%   u{i}.dc        = duty cycle ratio

% xs = past state values
%   xs{1} = current state
%   xs{2} = state t-1
%   ...
%   xs{n} = state t-n+1 
% params = simulation parameters
%   see ../NeedleSimulation.m for description of parameters
function xs = propagateNeedleForward(x, u, params)

% xs{1} = current state
xs{1} = x;
% uc = current control input
uc = u{1};
uc.dtheta = 0;

for i=2:length(u)
    xc = xs{i-1};
%     % reverse for propagating backward in time
%     xc.q = quatmult(xc.q, AxisAngleToQuat(pi*[0; 1; 0]));
    
    xc = propagateNeedleTip(xc,uc,params);
    xs{i} = xc;
    
    %xs{i}.q = quatmult(xs{i}.q, AxisAngleToQuat(pi*[0; 1; 0]));
    xs{i}.w = x.w;
    uc = u{i};
    uc.dtheta = u{i-1}.dtheta;
end
end