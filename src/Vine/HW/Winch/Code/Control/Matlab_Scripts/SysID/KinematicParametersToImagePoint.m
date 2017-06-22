function [pos,Z] = KinematicParametersToImagePoint(phi,theta,l,cameraParams)
t = cameraParams.targetPosition;

tip = kinematicParametersToTipPoint(l,phi,theta,eye(3),zeros(3,1));
axis = Rz(phi);
axis = axis(:,2)*theta;
RR = QuatToRotationMatrix(AxisAngleToQuat(axis));

f = cameraParams.focalLength;
C = [f 0 0 0;...
    0 f 0 0;...
    0 0 1 0];
E = [RR tip;...
    zeros(1,3) 1];
E = inv(E);

pos = C*E*[t;1];
Z = pos(3);
if Z < 0
    xx = 5;
end
pos = pos/pos(3);
pos = pos(1:2);
end