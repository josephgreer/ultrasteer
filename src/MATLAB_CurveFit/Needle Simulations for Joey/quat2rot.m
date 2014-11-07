function [X]=quat2rot(Q)
%Quaternion Representation
%Input:
%        Q: M X 3 matrix where each row is a set of quat. coefficients
%Output:
%        X: 3 x M matrix where each column is a rotation vector

% Determine angles
al = 2.*acos(Q(:,1));
% Determine unit vectors
U = Q(:,2:4)./repmat(sin(al/2),1,3);
% Multiply unit vectors by angle
X = (U.*repmat(al,1,3))';
% Set vectors corresponding to zero rotation to [0 0 0]
X(:,al == 0) = repmat([0 0 0]',1,numel( find( al == 0 ) ));
end