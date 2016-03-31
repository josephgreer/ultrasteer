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
function xs = propagateNeedleBack(x, u, params)

% xs{1} = current state
xs{1} = x;

% first proagate bevel
xs{2} = x;
xs{2}.pos = x.pos-QuatToRotationMatrix(x.q)*params.tipOffset;

% uc = current control input
uc = u{1};
uc.dtheta = 0;

length = 0;

i = 2;
while(length < params.minLength)
    xc = xs{i};
    % reverse for propagating backward in time
    xc.q = quatmult(xc.q, AxisAngleToQuat(pi*[0; 1; 0]));
    
    xc = propagateNeedleTip(xc,uc,params);
    xs{i+1} = xc;
    
    xs{i+1}.q = quatmult(xs{i+1}.q, AxisAngleToQuat(pi*[0; 1; 0]));
    xs{i+1}.w = x.w;
    if(i <= size(u,1))
        uc = u{i};
        uc.dtheta = u{i-1}.dtheta;
    else
        uc.v = 2;
        uc.dtheta = 0;
    end
    length = length+uc.v*params.dt;
    i=i+1;
end
end