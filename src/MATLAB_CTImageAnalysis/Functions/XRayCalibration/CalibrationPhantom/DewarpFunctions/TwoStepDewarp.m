function [dewarpedImg, dewarpParams] = TwoStepDewarp(img, convMaskSize, ...
    inputLineSearchMask, outputLineSearchMask, captureThresh, numLines, ...
    captureAngles, radiusSeparation, ...
    angleSeparation, modelVertLinesXs, modelHorzLinesYs, modelBoundingBox, ...
    polyFamily, degree, outputImgSize, outputMmPerPixel, precomputedBasis, ...
    showPixels, pauseCallbackHandle)
% function [dewarpedImg, dewarpParams] = TwoStepDewarp(img, convMaskSize, ...
%     inputLineSearchMask, outputLineSearchMask, captureThresh, numLines, ...
%     captureAngles, radiusSeparation, ...
%     angleSeparation, modelVertLinesXs, modelHorzLinesYs, modelBoundingBox, ...
%     polyFamily, degree, outputImgSize, outputMmPerPixel, precomputedBasis, ...
%     showPixels, pauseCallbackHandle)
%
% This is the main entrypoint for the dewarp method.  The outputs of this
% function are:
% \output dewarpedImg the input image, dewarped and resampled to the
% specified resolution.
% \output dewarpParams an object returned from maketform, containing the
% handle to the function BernsteinTransformPixels and the transformation
% parameters, that make the input image transform to the output image.  The
% object can be passed to imtransform to dewarp more images by the same
% parameters.  That is, for a new image, use
%
% >> dewarpedNewImage = imtransform(newImage, dewarpParams);
%
% For the inputs, see the functions:
%    * FindCoordinatesOfGridIntersections
%    * MakeBernsteinImageTransform
%    * FindWireGridLinesInGrayImage
%    * WarpWirePixelsFromRectifiedImage
%
%
% Example values for the parameters:
%
%   img - an image captured from the frame grabber; typically 480 rows by 640 columns
%   convMaskSize = [13, 13];
%   maskRadius = 220;
%   outputImgSizeRC = [480, 480];
%   outputImgCenterRC = (outputImgSizeRC + 1) / 2;
%   inputImgSizeRC = size(img);
%   inputImgCenterRC = (inputImgSizeRC + 1) / 2;
%   inputLineSearchMask = CreateOvalMask(size(img), ...
%       [outputImgCenterRC(2) - maskRadius, outputImgCenterRC(2) + maskRadius], ...
%       [outputImgCenterRC(1) - maskRadius, outputImgCenterRC(1) + maskRadius]);
%   outputLineSearchMask = CreateOvalMask(outputImgSize, ...
%       [outputImgCenterRC(2) - maskRadius, outputImgCenterRC(2) + maskRadius], ...
%       [outputImgCenterRC(1) - maskRadius, outputImgCenterRC(1) + maskRadius]);
%   captureThresh = 0.4;
%   numLines = 19;
%   captureAngles = 0:6;
%   radiusSeparation = 15;
%   angleSeparation = 4;
%   LoadWireGridGeometry;  % define desktop variables of wire grid coordinates
%   modelVertLineXs = modelVertRadii;
%   modelHorzLineYs = modelHorzRadii;
%   modelBoundingBox;
%   polyFamily = 'barycentric';
%   degree = 5;
%   outputImageSize;
%   outputMmPerPixel = [0.45 0.45];
%   precomputedBasis = [];   % advanced option to accelerate dewarping multiple images
%   showPixels = true;  % interactive process that shows intermediate results
%   pauseCallbackHandle = @pause;
%

%%% The value below is not related to the documentation above.
%   
% modelBoundingBox = [min(modelVertLinesXs) min(modelHorzLinesYs);...
%     max(modelVertLinesXs), max(modelHorzLinesYs)];

if (~exist('showPixels'))
    showPixels = 0;
end

if (~exist('pauseCallbackHandle'))
    pauseCallbackHandle = @pause;
end


[xModelCoords, yModelCoords, xPixelCoords, yPixelCoords] = ...
    FindCoordinatesOfGridIntersections(img, convMaskSize, ...
    inputLineSearchMask, captureThresh, numLines, captureAngles, radiusSeparation, ...
    angleSeparation, modelVertLinesXs, modelHorzLinesYs, showPixels, pauseCallbackHandle);
warp1Params = MakeBernsteinImageTransform(xModelCoords, yModelCoords, ...
    xPixelCoords, yPixelCoords, polyFamily, degree, modelBoundingBox, ...
    size(img), outputImgSize, outputMmPerPixel);

if (~isempty(precomputedBasis))
    warp1Params.tdata.precomputedBasis = precomputedBasis;
end

dewarp1Img = imtransform(img, warp1Params, 'xdata', [1, outputImgSize(2)], 'ydata', [1, outputImgSize(1)]);

[vertLinesDewarpedPixels, horzLinesDewarpedPixels, dewarpedVertRadii, dewarpedHorzRadii] = ...
    FindWireGridLinesInGrayImage(dewarp1Img, convMaskSize, outputLineSearchMask, captureThresh, ...
    numLines, [-3:3], radiusSeparation, 5, showPixels, pauseCallbackHandle);

[warpedVertPixels, warpedHorzPixels] = WarpWirePixelsFromRectifiedImage(...
    dewarp1Img, warp1Params, vertLinesDewarpedPixels, horzLinesDewarpedPixels, img);

vertFilter = ones(7, 5);
for lineCounter = 1:numLines
    lineImg = PutPixelsOnImage(zeros(size(img)), warpedVertPixels{lineCounter}, [0 0], 1);
    lineImg = conv2(lineImg, vertFilter, 'samesize');
    vertCluePixels{lineCounter} = find(lineImg > 5);
end

horzFilter = ones(5, 7);
for lineCounter = 1:numLines
    lineImg = PutPixelsOnImage(zeros(size(img)), warpedHorzPixels{lineCounter}, [0 0], 1);
    lineImg = conv2(lineImg, horzFilter, 'samesize');
    horzCluePixels{lineCounter} = find(lineImg > 5);
end

[vertLinesPixels2, horzLinesPixels2] = ...
    FindWireGridLinesInGrayImageWithCluePixels(img, convMaskSize, captureThresh, ...
    vertCluePixels, horzCluePixels, showPixels, pauseCallbackHandle);

vertLinesXs = AlignImageGridLinesWithModel(dewarpedVertRadii, modelVertLinesXs);
horzLinesYs = AlignImageGridLinesWithModel(dewarpedHorzRadii, modelHorzLinesYs);

[modelX, modelY, pixelX, pixelY] = ExtractIntersectionPixels(...
    size(img), vertLinesXs, horzLinesYs, vertLinesPixels2, horzLinesPixels2);

warp2Params = MakeBernsteinImageTransform(modelX, modelY, ...
    pixelX, pixelY, polyFamily, degree, modelBoundingBox, ...
    size(img), outputImgSize, outputMmPerPixel);
if (~isempty(precomputedBasis))
    warp2Params.tdata.precomputedBasis = precomputedBasis;
end


dewarp2Img = imtransform(img, warp2Params, 'xdata', [1, outputImgSize(2)], 'ydata', [1, outputImgSize(1)]);

dewarpedImg = dewarp2Img;
dewarpParams = warp2Params;
