function [ u ] = steeringPlanner( x, p )
%STEERINGPLANNER 
%   Determines (incremental) rotation angle, radius of curvature, and
%   incremental needle insertion to reach target from current needle pose.
%       - Takes current needle state vector {R, p} and
%       target position [x y z]' as inputs.
%       - Returns a vector containing d_th (incremental angle of rotation),
%       needle radius, and remaining distance to target

T = x;
% Define the tip position based on the input state
t = T(1:3,4);
% Define the target point relative to the 
p_rel1 = T\[p; 1];
p_rel1 = p_rel1(1:3);
% Find the projected unit vector of the point onto the x-y plane
v = p_rel1(1:2);
v = v./norm(v);
% Solve for the rotation angle
d_th = atan2(-v(1),v(2));
% Get the target in the rotated steering frame
p_rel2 = rotz(d_th)\[p_rel1; 1];
p_rel2 = p_rel2(1:3);
% Calculate the radius
r = sum(p_rel2.^2)/2/p_rel2(2);
% Define the center point in the base coordinates
c = T*(rotz(d_th)*[0 r 0 1]');
c = c(1:3);
% Also return the distance to the target in mm
l = min(norm(t-p),abs(t(3)-p(3)));
% Form as one input vector
u = [d_th; r; l];

end

