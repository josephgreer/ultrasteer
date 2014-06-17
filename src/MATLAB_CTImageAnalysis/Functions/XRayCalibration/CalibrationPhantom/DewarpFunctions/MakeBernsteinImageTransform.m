function TFORM = MakeBernsteinImageTransform(modelX, modelY, ...
    pixelX, pixelY, polyFamily, degree, modelBoundingBox, inputImageSize, ...
    outputImageSize, outputImageMmPerPixel)

% Normalize the model coordinate to the unit square
modelXScale = 1 / (modelBoundingBox(1,2) - modelBoundingBox(1,1));
modelYScale = 1 / (modelBoundingBox(2,2) - modelBoundingBox(2,1));
modelX = (modelX - modelBoundingBox(1,1)) * modelXScale;
modelY = (modelY - modelBoundingBox(2,1)) * modelYScale;

% Compute polynomial coefficients
[coefficientsX, coefficientsY] = ComputeModelToPixelPolynomialCoefficients(...
    modelX, modelY, pixelX, pixelY, polyFamily, degree);

% Compute the center of input image
centerRow = (1+inputImageSize(1)) / 2;
centerCol = (1+inputImageSize(2)) / 2;

% compute the center of the output image
outputCenterRow = (1+outputImageSize(1)) / 2;
outputCenterCol = (1+outputImageSize(2)) / 2;

% an affine transform to "centralize" output pixels
outputCentralizeTransform = [...
    1,   0,   -outputCenterCol;...
    0,   1,   -outputCenterRow;...
    0,   0,   1];

% an affine transform to scale centralized output pixels to mm units
outputToModelScaleTransform = [...
    outputImageMmPerPixel(2),     0,     0;...
    0,     outputImageMmPerPixel(1),     0;...
    0,                0,                 1];

% an affine transform to subtract low bounding box coordinates from mm
% units
modelToBoxTranslation = [...
    1,    0,    -modelBoundingBox(1,1);...
    0     1,    -modelBoundingBox(2,1);...
    0,    0,    1];

% an affine transform to scale mm's to unit square 
boxToUnitSquareScale = [...
    modelXScale,     0,      0;...
    0,     modelYScale,      0;...
    0,           0,          1];

% Compute pixel-to-model transform
outputPixelToModelAffineTransform = boxToUnitSquareScale * modelToBoxTranslation * ...
    outputToModelScaleTransform * outputCentralizeTransform;

% create a data object that stores all the parameters passed to
% BernsteinTransformPixels
tdata.outputPixelToModelAffineTransform = outputPixelToModelAffineTransform;
tdata.centerRow = centerRow;
tdata.centerCol = centerCol;
tdata.polyFamily = polyFamily;
tdata.degree = degree;
tdata.coefficientsX = coefficientsX;
tdata.coefficientsY = coefficientsY;

% Create a 'transform' object
TFORM = maketform('custom', 2, 2, [], @BernsteinTransformPixels, tdata);

