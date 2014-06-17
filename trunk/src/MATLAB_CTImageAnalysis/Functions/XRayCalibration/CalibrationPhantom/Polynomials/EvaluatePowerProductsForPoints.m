function v = EvaluatePowerProductsForPoints(points, powerIndex, powerBasis)
% function v = EvaluatePowerProductsForPoints(powerIndex, points)
% points is a m*n matrix. The rows of points are coordinates of points in n
% dimensions.
% powerIndex is a p*n matrix. The rows of powerIndex are powers of the
% corresponding variables in points. 
%
% For each point k, we evaluate v(k) = prod( points(k,j)^powerIndex(k,j) ) for
% j=1:n
%
% The result is a p*m matrix.  The rows correspond to the terms. the
% element (i,j) is the power product of the j-th point for the k-th term.

v = [];
if isempty(points) 
    %disp('PointsEmpty')
    return
end

[m, n] = size(points);
[numTerms, d] = size(powerIndex);

if d ~= n
    error('EvaluatePowerProductsForPoints: size of point must agree with size of powerIndex');
end
	
degree = max(sum(powerIndex, 2));

if nargin < 3
    powerBasis = GeneratePowerBasis(points, degree);
end

% termMultiplicands is a 3d matrix.  The layers of the matrix correspond to
% the terms of the polynomial.  The rows of each layer correspond to the
% points of evaluation.  The columns of each layer correspond to the
% coorinates of the point.
termMultiplicands = zeros(m, n, numTerms);

for varIndex = 1:n
    termPowers = powerIndex(:, varIndex);
    basisLocations = termPowers + 1;
    termMultiplicands(:, varIndex, :) = powerBasis(:, varIndex, basisLocations');
end

% v will be of size numTerms*m
powerProducts = prod( termMultiplicands, 2 );
v = reshape( powerProducts, [m, numTerms] )';
