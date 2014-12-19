% x = needle tip statee state
%   x.x0 % position of needle tip frame n timesteps back
%   x.R  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)

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
function xs = propagateNeedleBack(x, u, params)

% xs{1} = current state
xs{1} = x;
% uc = current control input
uc = u{1};
uc.dtheta = 0;

for i=2:length(u)
    xc = xs{i-1};
    % reverse for propagating backward in time
    R = QuatToRotationMatrix(xc.q);
    R(:,3) = -R(:,3);
    R(:,1) = -R(:,1);
    
    xc.q = RotationMatrixToQuat(R);
    
    xc = propagateNeedleTip(xc,uc,params);
    xs{i} = xc;
    
    R = QuatToRotationMatrix(xs{i}.q);
    R(:,3) = -R(:,3);
    R(:,1) = -R(:,1);
    xs{i}.q = RotationMatrixToQuat(R);
    uc = u{i};
    uc.dtheta = -u{i-1}.dtheta;
end
end