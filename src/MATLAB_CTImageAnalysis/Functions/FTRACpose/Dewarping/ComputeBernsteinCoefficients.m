% $Id: ComputeBernsteinCoefficients.m,v 1.1 2005/12/07 22:37:38 gouthami Exp $
function PolyCoeff = ComputeBernsteinCoefficients(Uobs, Unom, PolyOrder)
% function PolyCoeff = ComputeBernsteinCoefficients(Unom, Uobs, PolyOrder)
% ComputeBernsteinCoefficients finds the bernstein polynomial Coefficients
% C such that  Unom = Bezier(Uobs) * C. This function works on only two
% dimensional points(x,y)
% 
% Uobs - segmented/calculated pts ( normalized to image box)
% Unom - nominal/true pts
% PolyOrder - max degree of the polynomial to fit to the given points

% 
numDim1 = ndims(Unom);
numDim2 = ndims(Uobs);
if (numDim1 ~= numDim2 || numDim1 ~= 2)
    error (' Input matrices are not two dimensional ');
end

nCoeff = (PolyOrder +1)^numDim1;
nPts = size(Uobs, 1);
if (nPts < nCoeff)
    error(' too few points to fit %d order polynomial', PolyOrder);
end
% compute the bezier terms of Uobs
Bezier = ComputeBezierTerms(Unom, PolyOrder);

PolyCoeff = (Bezier' * Bezier) ^-1 * (Bezier' * Uobs);

% $Log: ComputeBernsteinCoefficients.m,v $
% Revision 1.1  2005/12/07 22:37:38  gouthami
% Adding the file to CVS for the first time
%