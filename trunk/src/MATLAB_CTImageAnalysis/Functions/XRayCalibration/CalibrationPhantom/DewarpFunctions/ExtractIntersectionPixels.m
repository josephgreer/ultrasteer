function [modelX, modelY, pixelX, pixelY] = ExtractIntersectionPixels(imgSize, ...
    vertLinesModelX, horzLinesModelY, vertLinesPixels, horzLinesPixels)
% function [modelX, modelY, pixelX, pixelY] = ExtractIntersectionPixels(imgSize, ...
%     vertLinesModelX, horzLinesModelY, vertLinesPixels, horzLinesPixels)
%
% This function has been factored out as a repeated task in the
% TwoStepDewarp function and its subroutines.  The function parameters are
% as follows.
%
% \param imgSize the size of the image where grid intersections are sought.
% (a 1*2 vector of the form [rows, cols])
% \param vertLinesModelX, horzLinesModelY the list of X and Y coordinates
% of the vertical and horizontal grid lines in the model, given in model
% units (mm).  These coordinates must be matched with the image line
% coordinates (next), before calling this function.  See
% AlignImageGridLinesWithModel for an example.
% \param vertLinesPixels, horzLinesPixels object arrays.  Each is a n_i*2
% array of [x, y] pixel coordinates with respect to the image center, where
% i is the index of the line detected on the image, on which the pixels are
% assumed to be.
%
% Method.
% The function only looks at pixels which are overlapped by both vertical
% and horizontal lines, that is, line intersections.  For each pixel that
% belongs to an intersection, the model X and Y coordinates are plotted in
% an auxiliary image structure.  Finally, all the coordinates of the
% intersection pixels only are returned in lists of corresponding elements:
% modelX, modelY, pixelX, pixelY for each of the intesection pixels.


numRows = imgSize(1);
numCols = imgSize(2);
centerRow = (numRows + 1) / 2;
centerCol = (numCols + 1) / 2;
modelXImage = NaN(numRows, numCols);
modelYImage = NaN(numRows, numCols);

for lineCounter=1:length(vertLinesPixels)
    linePixels = vertLinesPixels{lineCounter};
    modelXImage = PutPixelsOnImage(modelXImage, linePixels, [centerCol, centerRow], vertLinesModelX(lineCounter));
end

for lineCounter=1:length(horzLinesPixels)
    linePixels = horzLinesPixels{lineCounter};
    modelYImage = PutPixelsOnImage(modelYImage, linePixels, [centerCol, centerRow], horzLinesModelY(lineCounter));
end

[pixelXImage, pixelYImage] = meshgrid(1:numCols, 1:numRows);
pixelXImage = pixelXImage - centerCol;
pixelYImage = pixelYImage - centerRow;

pixelIndexes = find( (~isnan(modelXImage)) & (~isnan(modelYImage)) );
modelX = modelXImage(pixelIndexes);
modelY = modelYImage(pixelIndexes);
pixelX = pixelXImage(pixelIndexes);
pixelY = pixelYImage(pixelIndexes);

%% The following part has been commented out, and may be uncommented for
%% debugging.  showPixels can be made a function parameter.

% if (~showPixels)
%     return;
% end

% figure; imshow(modelXImage, []);
% figure; imshow(modelYImage, []);
% figure; imshow( (~isnan(modelXImage)) & (~isnan(modelYImage)) );
%% END DEBUG