function [coefficientsX, coefficientsY] = FindModelToPixelPolynomialCoefficients(...
    modelX, modelY, pixelX, pixelY, polyFamily, degree)
% function [coefficientsX, coefficientsY] = FindModelToPixelPolynomialCoefficients(...
%     modelX, modelY, pixelX, pixelY, polyFamily, degree)
%
% This function finds two column vectors of polynomial coefficients for
% X and Y polynomials that map [modelX, modelY] to pixelX and pixelY.
% The polynomial family is either 'tensor' or 'barycentric'.
% The values of modelX, modelY, pixelX, pixelY come, for example, from
% FindCoordinatesOfGridIntersections
matchFamily = 0;

basisMatrix = [];

if (strcmpi(polyFamily, 'tensor'))
    basisMatrix = ComputeBivariateBernsteinTensorBasisMatrix([modelX, modelY], degree);
    matchFamily = 1;
end

if (strcmpi(polyFamily, 'barycentric'))
    basisMatrix = ComputeBarycentricBernsteinBasisMatrix([modelX, modelY], degree);
    matchFamily = 1;
end

if (~matchFamily)
    msg = sprintf('Unknown polynomial family ''%s''', polyFamily);
    error(msg);
end

coefficientsX = basisMatrix \ pixelX;
coefficientsY = basisMatrix \ pixelY;
