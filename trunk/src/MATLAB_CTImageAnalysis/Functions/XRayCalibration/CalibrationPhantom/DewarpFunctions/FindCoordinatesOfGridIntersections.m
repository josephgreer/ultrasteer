function [modelX, modelY, pixelX, pixelY] = ...
    FindCoordinatesOfGridIntersections(image, convMaskSize, captureRegionBinaryMask, captureThresh, ...
    numLines, captureAngles, radiusSeparation, angleSeparation, ...
    modelVertLinesXs, modelHorzLinesYs, ...
    showPixels, pauseCallbackHandle)
% function [modelX, modelY, pixelX, pixelY] = ...
%     FindCoordinatesOfGridIntersections(image, convMaskSize, captureRegionBinaryMask, captureThresh, ...
%     numLines, captureAngles, radiusSeparation, angleSeparation, ...
%     modelVertLinesXs, modelHorzLinesYs, ...
%     showPixels, pauseCallbackHandle)
%
% This function returns four column vectors, containing the X and Y
% coordinates of corresponding model and image points.  The current
% correspondence of model point to image point is one-to-many.
%
% \param image a grayscale image to be processed.
%
% \param convMaskSize the size of convolution masks that search for line
% features in the image.  See FindWireGridLinesInGrayImage.
%
% \param captureRegionBinaryMask a binary image that is multiplying the pixel
% candidates to limit their location to a specific region of interest.  The
% capture region helps remove outliers that are detected near the edges of
% the image circle.  If this parameter is an empty array or a single
% scalar, then it is set to ones(size(image)).
%
% \param captureThresh a threshold (between 0 and 1) on the response to the
% line-pattern convolution filter.  The maximum response of the image,
% times this threshold, determined which pixels are identified as belonging
% to the line.
%
% \param numLines the actual number of lines to be extracted from the image
% using FindWireGridLinesInGrayImage
% \param captureAngles see FindWireGridLinesInGrayImage
% \param radiusSeparation see FindWireGridLinesInGrayImage
% \param angleSeparation see FindWireGridLinesInGrayImage
%
% \param modelVertLinesXs the X coordinates (in mm, etc.) of the vertical
% lines in the model.  The modelX output is a subset of this list.
% \param modelHorzLinesYs the Y coordinates (in mm, etc.) of the horizontal
% lines in the model.  The modelY output is a subset of this list.
% \param showPixels if set to true the function will show the pixels of the
% detected lines on a figure.
% \param pauseCallbackHandle see FindWireGridLinesInGrayImage
%
% The method is:
% 1. Find the pixels of the near-vertical and near-horizontal curves
% (near-lines) in the image, using FindWireGridLinesInGrayImage.
% 2. Align the found line radii (distances from the origin) with the model
% using AlignRadiiWithModel.  This returns the corresponding X and Y model
% coordinates of the pixels in each detected line.
% 3a. Plot the model coordinates as the pixels of images: modelXImage and
% modelYImage.
% 3b. Plot the pixel coordinates as images: pixelXImage and pixelYImage.
% 4. Select only the elements of the above four images that correspond to
% overlaps of the detected vertical and horizontal lines.  These elements,
% which are coordinates extracted from "ramp" images, constitue the output.
[numRows, numCols] = size(image);
imgRadius = min(numRows / 2, numCols / 2);
centerRow = (numRows + 1) / 2;
centerCol = (numCols + 1) / 2;

if (~exist('pauseCallbackHandle'))
    pauseCallbackHandle = @pause;
end

[vertLinesPixels, horzLinesPixels, vertRadii, horzRadii] = ...
    FindWireGridLinesInGrayImage(image, convMaskSize, captureRegionBinaryMask, captureThresh, ...
    numLines, captureAngles, radiusSeparation, angleSeparation, showPixels, pauseCallbackHandle);

vertLinesXs = AlignImageGridLinesWithModel(vertRadii, modelVertLinesXs);
horzLinesYs = AlignImageGridLinesWithModel(horzRadii, modelHorzLinesYs);

[modelX, modelY, pixelX, pixelY] = ExtractIntersectionPixels(...
    [numRows, numCols], vertLinesXs, horzLinesYs, vertLinesPixels, horzLinesPixels);
