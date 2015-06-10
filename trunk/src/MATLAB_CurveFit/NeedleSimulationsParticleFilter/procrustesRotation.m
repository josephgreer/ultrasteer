% finds the optimal rotation bringing
% X(i,:) into Y(i,:)
% for i=1,...,size(X,1)
function R = procrustesRotation(X,Y)
X = X';
Y = Y';

[U S V] = svd(X*Y');
R = V*U';
if(det(R) < 0)
    R = V*diag([1 1 -1])*U';
end
end