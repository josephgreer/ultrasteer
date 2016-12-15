function [phi, theta] = PositionFrameToPhiTheta(x, x0, R0, plane)
p1 = projectPointOntoPlane(plane, x.pos);

d1 = p1-x0.pos;

phi = atan2(dot(R0(:,2),d1), dot(R0(:,1),d1));
phi = mod(phi, 2*pi);
R = QuatToRotationMatrix(x.q);

theta = acos(dot(R(:,3),R0(:,3)));
theta = mod(theta, 2*pi);
end