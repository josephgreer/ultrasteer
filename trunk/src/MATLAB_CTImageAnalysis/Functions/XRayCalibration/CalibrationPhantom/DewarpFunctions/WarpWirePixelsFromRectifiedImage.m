function [warpedVertPixels, warpedHorzPixels] = WarpWirePixelsFromRectifiedImage(rectImg, warpParams, vertLinePixels, horzLinePixels, warpImg)
% function [warpedVertPixels, warpedHorzPixels] = WarpWirePixelsFromRectifiedImage(rectImg, warpParams, vertLinePixels, horzLinePixels, warpImg)
% 
% This is an auxiliary function that projects pixels that were detected on
% lines in a *dewarped* image back to their warped positions.
%
% \input rectImage the dewarped image
% \input warpParams an object that contains the warp parameters, e.g.,
% returned from MakeBernsteinImageTransform
% \input vertLinePixels an object array containing N*2 arrays of pixel
% coordinates on the vertical wires, detected in the *dewarped image*,
% e.g., by FindWireGridLinesInGrayImage . The coordinates are with respect
% to the image center.
% \input horzLinePixels an object array containing N*2 arrays of pixel
% coordinates on the horizontal wires.  See vertLinePixels.
% \input warpImg the warped image, before correction.
%
% \output warpedVertPixels an object array containing the warped
% coordinates of vertLinePixels, with respect to the "top-left" corner of
% warpImg (1-based).
% \output warpedHorzPixels an object array containing the warped
% coordinates of horzLinePixels, with respect to the "top-left" corner of
% warpImg (1-based).


[numOutRows, numOutCols] = size(rectImg);
[warpRows, warpCols] = size(warpImg);
rectCenterRow = (numOutRows + 1) / 2;
rectCenterCol = (numOutCols + 1) / 2;

numPixelsForEachLine = [];
allLinePixels = [];
numLines = length(vertLinePixels);
for lineCounter=1:numLines
    linePixels = vertLinePixels{lineCounter};
    numPixelsForEachLine = [numPixelsForEachLine, size(linePixels, 1)];
    allLinePixels = [allLinePixels; linePixels];
end

numLines = length(horzLinePixels);
for lineCounter=1:numLines
    linePixels = horzLinePixels{lineCounter};
    numPixelsForEachLine = [numPixelsForEachLine, size(linePixels, 1)];
    allLinePixels = [allLinePixels; linePixels];
end

allLinePixelsSub = [allLinePixels(:,1) + rectCenterCol, allLinePixels(:,2) + rectCenterRow];

if (isfield(warpParams.tdata, 'precomputedBasis'))
    warpParams.tdata = rmfield(warpParams.tdata, 'precomputedBasis');
end

allWarpedPixels = BernsteinTransformPixels(allLinePixelsSub, warpParams);

numLines = length(vertLinePixels);
for lineCounter=1:numLines
    warpedVertPixels{lineCounter} = allWarpedPixels(1:numPixelsForEachLine(1),:);
    allWarpedPixels(1:numPixelsForEachLine(1),:) = [];
    numPixelsForEachLine(1) = [];
end

numLines = length(horzLinePixels);
for lineCounter=1:numLines
    warpedHorzPixels{lineCounter} = allWarpedPixels(1:numPixelsForEachLine(1),:);
    allWarpedPixels(1:numPixelsForEachLine(1),:) = [];
    numPixelsForEachLine(1) = [];
end
