function outImg = PutPixelsOnImage(img, pixelCoords, coordOffset, pixelColor)
% function outImg = PutPixelsOnImage(img, pixelCoords, coordOffset, pixelColor)
% 
% Highlight the input pixels, given as a N*2 array, with the first column
% representing the X coordinate (image column) and the second the Y
% coordinate (image row).  The coordOffset is added to all components of
% pixelCoord.  The highlighting is by setting the pixels in the output
% image to the specified color.
numComponents = length(pixelColor);
numImageComponents = size(img, 3);
numRows = size(img, 1);
numCols = size(img, 2);
outImg = zeros([numRows, numCols, numComponents] );

numPixels = size(pixelCoords, 1);
roundPixelCoords = round(pixelCoords + repmat(coordOffset, [numPixels, 1]));
outOfBoundIndexes = find( ...
    (roundPixelCoords(:,1) < 1) | (roundPixelCoords(:,1) > numCols) | ...
    (roundPixelCoords(:,2) < 1) | (roundPixelCoords(:,2) > numRows) );
roundPixelCoords(outOfBoundIndexes, :) = [];

pixIdx = sub2ind( [numRows, numCols], roundPixelCoords(:,2), roundPixelCoords(:,1) );

for cmp = 1:numComponents
    tmpImg = img(:,:, min(cmp, numImageComponents));
    tmpImg(pixIdx) = pixelColor(cmp);
    outImg(:,:,cmp) = tmpImg;
end

