function resamplePixels = BernsteinTransformPixels(outputPixels, T)
% function resamplePixels = BernsteinTransformPixels(outputPixels, T)
%
% This function is used to transform pixel coordinates in the output image
% to resampling coordinates in an input image, where the transformation is
% a Bernstein polynomial (in tensor or barycentric form).  Typically, this
% function serves as a callback from imtransform, which generates the
% outputPixels as a N*2 array.  T is created using maketform, where a
% handle of this function is stored in the INVERSE_FCN member.  T.tdata
% member should contain the following information:
%
% outputPixelToModelAffineTransform a 3x3 matrix representing an affine 2D
% transform in homogeneous coordinates.  The transform is applied to a
% column of 2 output pixel coordinates and 1 homogeneous coordinate.
% Typically, it includes translation and scale from the output image pixels
% to the "model" domain in mm or another unit (e.g., normalized).
%
% centerRow, centerCol the 1-based pixel coordinates of the input image
% center (possibly noninteger)
%
% polyFamily either 'tensor' or 'barycentric'.  See
% ComputeBivariateBernsteinTensorBasisMatrix and
% ComputeBarycentricBernsteinBasisMatrix
%
% degree
%
% coefficientsX a column vector of the coefficients of all the basis
% functions comprising the polynomial for the X component
%
% coefficientsY a column vector of the coefficients of all the basis
% functions comprising the polynomial for the Y component
%
% precomputedBasis is an optional field that skips the internal evaluation
% of the basis functions altogether by using this matrix, which is the
% result of applying transformation to pixel coordinates.
%
% The output is a N*2 array of 1-based coordinates that can be used to
% interpolate pixel values in the input image domain of imtransform.
%
%
% NOTE: The polynomials are typically defined with respect to the center
% of the model being mapped to the "central" feature in the input image.
% The polynomial values are in pixel units with respect to the exact center
% of the input image.  Therefore, centerCol and centerRow are added to the
% computed X and Y values from the polynomials to form the final output.
%
pixelToModelAffineTransform = T.tdata.outputPixelToModelAffineTransform;
polyFamily = T.tdata.polyFamily;
centerRow = T.tdata.centerRow;
centerCol = T.tdata.centerCol;
degree = T.tdata.degree;
coefficientsX = T.tdata.coefficientsX;
coefficientsY = T.tdata.coefficientsY;

numPixels = size(outputPixels, 1);

polynomialBasisMatrix = [];

if (isfield(T.tdata, 'precomputedBasis'))
    polynomialBasisMatrix = T.tdata.precomputedBasis;
else
    modelCoordinates = [outputPixels, ones(numPixels, 1)] * pixelToModelAffineTransform';
    modelCoordinates(:,1:2) = modelCoordinates(:,1:2) ./ [modelCoordinates(:,3) * ones(1,2)];
    modelCoordinates(:,3) = [];


    if (strcmpi(polyFamily, 'tensor'))
        polynomialBasisMatrix = ComputeBivariateBernsteinTensorBasisMatrix(modelCoordinates, degree );
    end

    if (strcmpi(polyFamily, 'barycentric'))
        polynomialBasisMatrix = ComputeBarycentricBernsteinBasisMatrix(modelCoordinates, degree );
    end
end  %% 'precomputedBasis' condition

if (isempty(polynomialBasisMatrix))
    msg = sprinf('Unknown polynomial family string: ''%s''', polyFamily);
    error(msg);
end

newXs = polynomialBasisMatrix * coefficientsX;
newYs = polynomialBasisMatrix * coefficientsY;

resamplePixels = [newXs + centerCol, newYs + centerRow];
