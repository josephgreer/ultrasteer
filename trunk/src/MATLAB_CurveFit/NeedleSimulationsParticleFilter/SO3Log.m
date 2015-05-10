% computes the log map from SO(3)->so(3)
% that is, rotation to axis angle
% assumes R is a matrix
% returns a skew symmetric matrix x
function X = SO3Log(R)
theta = acos((trace(R)-1)/2);
if(theta < 1e-6)
    X = zeros(3,3);
    return;
end
X = theta/(2*sin(theta))*(R-R');
end