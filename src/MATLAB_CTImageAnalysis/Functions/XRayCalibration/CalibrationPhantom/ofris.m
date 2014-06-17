clc
addpath('DewarpFunctions')
addpath('GeneralFunctions')
addpath('ImageCalibrationFunctions')
addpath('Models')
addpath('Polynomials')
img = imread('CalibrationPattern.jpg');
img = double(img(:,:,1)) / 255;
convMaskSize = [13, 13];
maskRadius = 220;
outputImgSizeRC = [480, 480];
outputImgCenterRC = (outputImgSizeRC + 1) / 2;
inputImgSizeRC = size(img);
inputLineSearchMask = CreateOvalMask(size(img), [124 613], [14, 460]);
outputLineSearchMask = CreateOvalMask(outputImgSizeRC, ...
[outputImgCenterRC(2) - maskRadius, outputImgCenterRC(2) + maskRadius], ...
[outputImgCenterRC(1) - maskRadius, outputImgCenterRC(1) + maskRadius]);
captureThresh = 0.4;
numLines = 19;
captureAngles = 0:6;
radiusSeparation = 15;
angleSeparation = 4;
LoadWireGridGeometry;  % define desktop variables of wire grid coordinates
modelVertLineXs = modelVertRadii;
modelHorzLineYs = modelHorzRadii;
modelBoundingBox;
polyFamily = 'barycentric';
degree = 5;
outputMmPerPixel = [0.45 0.45];
precomputedBasis = [];   % advanced option to accelerate dewarping multiple images
showPixels = false;  % interactive process that shows intermediate results
pauseCallbackHandle = @pause;
[dewarpedImg, dewarpParams] = TwoStepDewarp(img, convMaskSize, inputLineSearchMask, outputLineSearchMask, captureThresh, numLines, captureAngles, radiusSeparation, angleSeparation, modelVertLineXs, modelHorzLineYs, modelBoundingBox, polyFamily, degree, outputImgSizeRC, outputMmPerPixel, precomputedBasis, showPixels, pauseCallbackHandle);
imshow(dewarpedImg)
figure
imshow(img)