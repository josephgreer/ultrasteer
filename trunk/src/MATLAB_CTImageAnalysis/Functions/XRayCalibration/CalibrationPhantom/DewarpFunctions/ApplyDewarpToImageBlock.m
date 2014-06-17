function maskedDewarpedImgBlock = ApplyDewarpToImageBlock(...
    imgBlock, dewarpParams, dewarpSizes, maskXRange1, maskYRange1, ...
    useLog, maskXRange2, maskYRange2, saturationIntensity)
%
% function maskedDewarpedImgBlock = ApplyDewarpToImageBlock(...
%    imgBlock, dewarpParams, dewarpSizes, maskXRange1, maskYRange1, ...
%    useLog, maskXRange2, maskYRange2, saturationIntensity)
%
% This function dewarps a block of images based on known dewarp parameters.
% It also masks the results, and optionally takes the log of the dewarped
% images.
%
% \param imgBlock a 3D array whose slices are the images to dewarp.
%
% \param dewarpParams an object array.  Each element represents the
% parameters for dewarping the corresponding image, through Matlab's
% imtransform function
%
% \param dewarpSizes a 2 element vector containing the numbers of rows and
% columns (in this order) in the output dewarped images.
%
% \param maskXRange1,maskYRange1 each is a 2 element vector that specifies
% the first and last pixel coordinates (1-based) in the X (columns) and Y
% (rows) directions of an elliptic mask.  The mask array size is the same
% as the dewarped image, and only pixels inside the ellipse are returned.
% The first mask is applied directly to the dewarped image.
%
% \param useLog if false, the dewarped and masked image is returned
% directly.  Otherwise the process continues.
%
% \param maskXRange2,maskYRange2 mask parameters for a second mask, applied
% to the log of the dewarped image.  This is done to reduce the residues of
% pixels at the boundary of the first mask, which are not completely
% saturated and therefore leave traces when the log is taken.
%
% \param saturationIntensity a scalar for taking the log of the image:
% log(saturationIntensity ./ dewarpedImage).
%
% \output the dewarped and masked image.

numImages = size(imgBlock, 3);

dewarpedBlock = zeros([dewarpSizes(1) dewarpSizes(2) numImages]);
for k=1:numImages
    dewarpedBlock(:,:,k) = imtransform(imgBlock(:,:,k), dewarpParams{k}, ...
        'xdata', [1 dewarpSizes(2)], 'ydata', [1 dewarpSizes(1)]);
end

maskXCenter = (maskXRange1(1) + maskXRange1(2)) / 2;
maskXRadius = (maskXRange1(2) - maskXRange1(1)) / 2;
maskYCenter = (maskYRange1(1) + maskYRange1(2)) / 2;
maskYRadius = (maskYRange1(2) - maskYRange1(1)) / 2;

[maskXCoord, maskYCoord] = meshgrid([(-maskXCenter):(-maskXCenter + dewarpSizes(2) - 1)], ...
    [(-maskYCenter):(-maskYCenter + dewarpSizes(1) - 1)]);

mask1 = (maskXCoord .* maskXCoord) + (maskXRadius / maskYRadius)^2 * (maskYCoord .* maskYCoord) ...
    < maskXRadius^2;

maskedDewarpedImgBlock = dewarpedBlock .* repmat(mask1, [1 1 numImages]);

if ~useLog
    return
end

clear imgBlock dewarpedBlock

maskXCenter = (maskXRange2(1) + maskXRange2(2)) / 2;
maskXRadius = (maskXRange2(2) - maskXRange2(1)) / 2;
maskYCenter = (maskYRange2(1) + maskYRange2(2)) / 2;
maskYRadius = (maskYRange2(2) - maskYRange2(1)) / 2;

[maskXCoord, maskYCoord] = meshgrid([(-maskXCenter):(-maskXCenter + dewarpSizes(2) - 1)], ...
    [(-maskYCenter):(-maskYCenter + dewarpSizes(1) - 1)]);

mask2 = (maskXCoord .* maskXCoord) + (maskXRadius / maskYRadius)^2 * (maskYCoord .* maskYCoord) ...
    < maskXRadius^2;

prelogBlock = maskedDewarpedImgBlock;
prelogBlock(find(prelogBlock == 0)) = saturationIntensity;
logBlock = log( saturationIntensity ./ prelogBlock );

maskedDewarpedImgBlock = logBlock .* repmat(mask2, [1 1 numImages]);

