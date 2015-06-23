% X = [x1; x2; x3; ...] x_i \in R^3
% Y = [y1; y2; y3; ...] y_i \in R^3

% D = D_ij = |(x_i-y_j)|^2
function D = distanceMatrix(X,Y)
M = size(X,1); N = size(Y,1);
D = repmat(diag(X*X'),1,N)-2*X*Y'+repmat(diag((Y*Y'))', M, 1);
end