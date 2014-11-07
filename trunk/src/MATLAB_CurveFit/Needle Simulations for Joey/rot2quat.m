function [Q]=rot2quat(X)
%Quaternion Representation
%Input:
%        X: 3 x M matrix where each column is a rotation vector
%Output:
%        Q: M X 4 matrix where each row is a set of quat. coefficients

% Check size of input vector
[k,m] = size(X);
if (k ~= 3)
    display('Error incorrect input vector');
end
% Detemine norm of each vector and normalize
al = sum(X.^2,1).^0.5;
e = X./repmat(al,3,1);
% Transform to quaternion representation
Q = [cos(al./2)' e'.*repmat(sin(al'./2),1,3)];
% Set quaternions corresponding to zero rotation norm to [1 0 0 0]
Q(al == 0,:) = repmat([1 0 0 0],numel( find( al == 0 ) ),1);
end