% computes the exp map from so(3)->SO(3)
% that is, axis angle to rotation matrix
% assumes X is a matrix or vector (3x1)
% returns a rotation matrix corresponding to exp(X)
% using rodrigues formula
function R = SO3Exp(X)
x = X;
if(size(X,1) == 1 || size(X,2) == 1)
    X = SO3Hat(X);
else
    x = SO3HatInverse(X);
end

theta = norm(x);
K = SO3Hat(x/theta);
R = eye(3)+sin(theta)*K+(1-cos(theta))*K*K;
end