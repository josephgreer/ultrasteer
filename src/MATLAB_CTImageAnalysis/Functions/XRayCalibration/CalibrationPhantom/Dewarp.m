function [dewarpedImg, dewarpParams] = Dewarp(img)
%clc;
%close all;
%addpath('DewarpFunctions')
%addpath('GeneralFunctions')
%addpath('ImageCalibrationFunctions')
%addpath('Models')
%addpath('Polynomials')
%img = imread('CalibrationPattern.jpg');
%newImage = imread('023dia20ins1.jpg');

img = double(img(:,:,1)) / 255;
%newImage = double(newImage(:,:,1)) / 255;

convMaskSize = [13, 13];
maskRadius = 220;
outputImgSizeRC = [480, 480];
outputImgCenterRC = (outputImgSizeRC + 1) / 2;
inputImgSizeRC = size(img);
inputLineSearchMask = CreateOvalMask(size(img), [124 613], [14, 460] ); %[122 615], [12, 462]
outputLineSearchMask = CreateOvalMask(outputImgSizeRC, ...
[outputImgCenterRC(2) - maskRadius, outputImgCenterRC(2) + maskRadius], ...
[outputImgCenterRC(1) - maskRadius, outputImgCenterRC(1) + maskRadius]);
captureThresh = 0.4;
numLines = 19; %19 orig
captureAngles = -3:3; %0:6 orig, -3:3 done
radiusSeparation = 15; %15 orig
angleSeparation = 4; % 4 orig
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
%imshow(dewarpedImg);
% figure
% imshow(img);
% 
%  dewarpedNewImage = imtransform(newImage, dewarpParams);
%  figure
%  imshow(dewarpedNewImage);
%  figure
% imshow(newImage);
% imwrite(dewarpedImg,'DewarpedImage.tif');