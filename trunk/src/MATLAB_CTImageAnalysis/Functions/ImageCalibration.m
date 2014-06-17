function [prm, linesA, linesB] = ImageCalibration(image)

%addpath('XRayCalibration/CalibrationPhantom')
%addpath('XRayCalibration/CalibrationPhantom/DewarpFunctions')
%addpath('XRayCalibration/CalibrationPhantom/GeneralFunctions')
%addpath('XRayCalibration/CalibrationPhantom/ImageCalibrationFunctions')
%addpath('XRayCalibration/CalibrationPhantom/Models')
%addpath('XRayCalibration/CalibrationPhantom/Polynomials')

image = im2double(image);

convMaskSize = [13, 13];
radiusSeparation = 8; %ORIGNALY 15. This parameter defines how close together the lines are - in my images two lines are closer together so it fails with 15
angleSeparation = 4;
LoadCalibrationPatternGeometry;
modelPoints = calibrationPhantomPoints;
%pixelSize = [480 480]; %size(image)??
pixelSize = [0.45 0.45];
showPixels = false;
showLines = false;
lineStyle = ':';
showIntersections = false;
noise = 0;

[prm, linesA, linesB] = CalibrateFromImage(image, convMaskSize, ...
    radiusSeparation, angleSeparation, modelPoints, pixelSize, showPixels, ...
    showLines, lineStyle, showIntersections, noise);
end

