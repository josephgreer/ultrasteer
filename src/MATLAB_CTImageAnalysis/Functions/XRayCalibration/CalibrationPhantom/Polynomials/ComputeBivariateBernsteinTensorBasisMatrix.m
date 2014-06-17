function M = ComputeBivariateBernsteinTensorBasisMatrix(points, degree)
% function M = ComputeBivariateBernsteinTensorBasisMatrix(points, degree)
%
% This function creates a matrix that includes all the values of the
% Bernstein basis functions in tensor form for all the given input points.
% The input points are given as a N*2 matrix, where N is the number of
% points and 2 is the dimension of space.  The basis functions are
% evaluated over 4 components for each point (x,y): x, 1-x, y, 1-y.
%
% The shape and order of the basis functions is described in
% GenerateBivariateBernsteinTensorBasis.
%
% The return value is a N*M matrix, with each row corresponding to an input
% point. M = (degree+1)^2.
%
% Note that the polynomial is evaluated for elements in the 4D space,
% and not in the original 2d space of the input points.
%

[numPoints, dimensions] = size(points);

[multinomialFactors, powersMatrix] = GenerateBivariateBernsteinTensorBasis(degree);

M = EvaluatePowerProductsForPoints([points(:,1), 1-points(:,1), points(:,2), 1-points(:,2)], powersMatrix);

MFMatrix = ones(numPoints, 1) * multinomialFactors';

M = MFMatrix .* M';
