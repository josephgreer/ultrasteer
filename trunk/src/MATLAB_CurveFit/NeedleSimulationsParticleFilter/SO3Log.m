% computes the log map from SO(3)->so(3)
% that is, rotation to axis angle
% assumes R is a matrix
% returns a skew symmetric matrix x
function X = SO3Log(R)
X = SO3Hat(QuatToAxisAngle(RotationMatrixToQuat(R)));
end