clear; clc; close all;

R = Ry(1)*Ry(1)

q = RotationMatrixToQuat(R)
Rt = QuatToRotationMatrix(q)

v = QuatToAxisAngle(q)
q = AxisAngleToQuat(v)

v = [1; 2; 3];
R = Rx(rand(1))*Ry(rand(1))*Rz(rand(1));
q = RotationMatrixToQuat(R);

yep = quatmult(q,quatinverse(q))

vr = R*v
vq = quatrot(q,v)

R = eye(3);
q = RotationMatrixToQuat(R)
R = QuatToRotationMatrix(q)
