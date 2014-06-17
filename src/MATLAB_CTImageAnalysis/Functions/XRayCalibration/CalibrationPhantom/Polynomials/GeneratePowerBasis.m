function PB = GeneratePowerBasis(points, degree)
% function PB = GeneratePowerBasis(points, degree)
% A point is given as 1*n row vector, points is given as m*n matrix.
% for each point [x y ... q], we generate the power basis:
%   x^0   x^1   x^2  ...  x^degree
%   y^0   y^1   y^2  ...  y^degree
%   ...   ...   ...  ...  ...
%   q^0   q^1   q^2  ...  q^degree
%
% We return a m*n*degree 3-dimensional matrix. Layer i of PB contains the
% i-th powers of [x y ... q] of each point in the corresponding row and
% column locations.
[m, n] = size(points);

PB = ones(m, n);
for i = 1:degree
    PB(:,:,i+1) = points .* PB(:,:,i);
end

