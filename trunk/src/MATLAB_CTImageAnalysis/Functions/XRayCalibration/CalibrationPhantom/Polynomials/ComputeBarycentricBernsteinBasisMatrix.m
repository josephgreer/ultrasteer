function M = ComputeBarycentricBernsteinBasisMatrix(points, degree)
% function M = ComputeBarycentricBernsteinBasisMatrix(points, degree)
%
% This function creates a matrix that includes all the values of the
% barycentric Bernstein basis functions for all the given input points.
% The input points are given as a N*d matrix, where N is the number of
% points and d is the dimension of space.  The points need not be
% barycentric, that is, the sum of the components of each point doesn't
% have to be 1.  However, the basis functions are evaluated over d+1
% components for each point, where the first d are the original input
% point, and the last balances the sum of the components to 1.  Every basis
% function has the shape
%
%  f_{p_1, p_2, ..., p_d, p_{d+1}} * (x_1)^{p_1} * (x_2)^{p_2} * ... *
%  (x_d)^{p_d} * (x_{d+1})^{p_{d+1}}
%
% where (p_1, p_2, ..., p_d, p_{d+1}) are the "power index" of the basis
% function; f is a multinomial factor; the sum of the components
% x_1+x_2+...+x_d+x_{d+1}=1; the sum of the powers
% p_1+p_2+...+p_d+p_{d+1}=P is equal to the input degree.
%
% The return value is a N*M matrix, with each row corresponding to an input
% point, and each element contains the value of one basis function for that
% point.  The order of terms is defined in GenerateCisstBernsteinBasis.
%
% Note that the polynomial is evaluated for elements in the d+1 barycentric
% space, and not in the original d-space of the input points.
%

[numPoints, dimensions] = size(points);

barycentricCol = 1 - sum(points, 2);
barycentricPts = [points, barycentricCol];

[multinomialFactors, powersMatrix] = GenerateCisstBernsteinBasis(dimensions+1, degree);

M = EvaluatePowerProductsForPoints(barycentricPts, powersMatrix);
%size(M)

MFMatrix = ones(numPoints, 1) * multinomialFactors';
%size(MFMatrix)
M = MFMatrix .* M';
