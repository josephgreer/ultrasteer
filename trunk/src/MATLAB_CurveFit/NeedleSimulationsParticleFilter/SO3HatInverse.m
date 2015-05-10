% computes the inverse of the hat map from so(3)->R^3
% that is, takes a skew symmetric matrix in so(3)
% returns the vecotr in R^3
function x = SO3Hat(X)
x = [X(3,2); -X(3,1); -X(1,2)];
end