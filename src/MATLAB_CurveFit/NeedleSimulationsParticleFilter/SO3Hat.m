% computes the hat map from R^3->SO(3)
% that is, takes a vector in R^3 and
% returns the cross product matrix
function X = SO3Hat(x)
X = [0 -x(3) x(2); x(3) 0 -x(1); -x(2) x(1) 0];
end