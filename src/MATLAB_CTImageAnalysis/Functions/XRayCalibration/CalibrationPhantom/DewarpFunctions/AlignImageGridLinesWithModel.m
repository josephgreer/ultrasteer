function actualModelRadii = AlignImageGridLinesWithModel(imageRadii, modelRadii)
% function actualModelRadii = AlignImageGridLinesWithModel(imageRadii, modelRadii)
%
% This function matches the vertical or horizontal lines found in an image,
% given as signed distances from the image center in pixel units, with
% lines in the model, given as signed distances from the center of the
% model in model units.  The first assumption is that the image lines are
% distibuted evenly around the center line, and given in sorted order.
% Therefore, the center line represents the median in the given list, which
% is also the middle element.
% Second assumption is that there are no breaks or duplications in the
% sequence of lines found in the image.  That is, they correspond exactly
% to a contiguous subset of the model lines.  In that sense, it depends on
% the quality of image segmentation.
%
% The function returns the list of model coordinates which presumably
% correspond to the given image coordinates, based on the above
% assumptions.

% Former assumption, no longer used, is that the image line closest to
% the center, that is, with the smallest-magnitude signed distance,
% corresponds to the line at the center of the model, with zero distance.

numImageRadii = length(imageRadii);
numModelRadii = length(modelRadii);

if (numImageRadii > numModelRadii)
    error('Too many image radii');
end

absImgRad = abs(imageRadii);
modelZeroIdx = find((modelRadii == 0), 1);

% To form the former assumption, we used the following on the right-hand
% side of the line below:
%      find( (absImgRad == min(absImgRad)), 1 );
imageSmallestRadiusIdx = (numImageRadii + 1) / 2;

indexShiftAmt = modelZeroIdx - imageSmallestRadiusIdx;

firstModelIdx = 1 + indexShiftAmt;
lastModelIdx = firstModelIdx + numImageRadii - 1;

if ( (firstModelIdx < 1) | (lastModelIdx > numModelRadii) )
    error('Cannot align data');
end

actualModelRadii = modelRadii(firstModelIdx:lastModelIdx);
