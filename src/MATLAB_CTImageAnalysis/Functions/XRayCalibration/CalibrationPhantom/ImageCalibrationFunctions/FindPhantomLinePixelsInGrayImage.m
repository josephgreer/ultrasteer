function [lines1Pixels, lines2Pixels] = FindPhantomLinePixelsInGrayImage(image, convMaskSize, ...
    radiusSeparation, angleSeparation, showPixels)
% function [lines1Pixels, lines2Pixels] = FindPhantomLinePixelsInGrayImage(image, convMaskSize, ...
%     radiusSeparation, angleSeparation, showPixels)
% Find two sets of lines, one at approximately 45 degree angle and one at
% approximately -45 degree angle.  Each set contains 7 segments: three on a
% line that goes approximately through the origin, and four more
% off the origin.  The general pattern is somewhat like
%
%             |
%        -----------
%      |    -----    |
%      |  |   |   |  |
%  --- |  | --+-- |  | ---
%      |  |   |   |  |
%      |    -----    |
%        -----------
%             |
%
% rotated 45 degrees.
%
% \param image a grayscale image that contains the pattern as dark lines on
% bright background.  Eg., and x-ray image.
%
% \param convMaskSize the size of convolution mask to find candidates for
% line pixels.  See the code for more information.
% \param radiusSeparation, angleSeparation A number specifying the
% separation between peaks in the Hough transform of the line pattern,
% which detects the individual lines.  Given in pixel units.  See
% FindPhantomAnglesAndRadii.
%
% \param showPixels if nonzero then each detected segment is displayed in a
% figure.
%
% \output lines1Pixels an object array with 7 components, each containing
% the coordinates of the pixels on one of the line segments, and all on
% lines in one direction (45 degrees with respect to the col/row index
% axes).  The pixel coordinates are in X-Y, x being the column from left to
% right, and Y being the row from low-index (typically top) to high index.
% The coordinates are counted from the image center, that is, the top-left
% corner of the image has all negative coordinates.  The line segmentes are
% sorted so that the radius (signed distance of the line from the origin
% along its normal) is increasing, except for the outer two segments on the
% distal plate, which are elements 6 and 7 in the output.
% \output lines2Pixels a similar array of the pixels on the lines in angle
% of -45 degrees.



% The masks represent diagonal black lines on a white background.  The mean
% is subtracted so that the sum of elements is zero, which makes it a
% "high-pass" filter.  They detect pixels on lines of the 45 degree or -45
% degree angle.
% The results of the convolution are thresholded to obtain pixels with high
% correlation.
mask1 = ones(convMaskSize) - eye(convMaskSize);
mask1 = mask1 - mean(mean(mask1));
mask2 = mask1(convMaskSize:-1:1,:);
[numRows, numCols] = size(image);
imgRadius = min(numRows / 2, numCols / 2);
centerRow = (numRows + 1) / 2;
centerCol = (numCols + 1) / 2;

threshold = max(max(image)) * 0.5;
numLines = 5;

img1 = conv2(image, mask1, 'samesize');
binImg1 = (img1 > threshold);
img1PixelsInd = find(binImg1);
[img1PixRows, img1PixCols] = ind2sub(size(img1), img1PixelsInd);
centeredPix1Rows = img1PixRows - centerRow;
centeredPix1Cols = img1PixCols - centerCol;

img2 = conv2(image, mask2, 'samesize');
binImg2 = (img2 > threshold);
img1PixelsInd = find(binImg2);
[img2PixRows, img2PixCols] = ind2sub(size(img2), img1PixelsInd);
centeredPix2Rows = img2PixRows - centerRow;
centeredPix2Cols = img2PixCols - centerCol;

% The Hough transform parametrizes lines by their normal angle and radius,
% i.e., distance from the origin.  It counts how many pixels are incident
% on each such line.  We select 5 peaks for each direction, which represent
% the 7 segments, three of which are nearly collinear.
angles1 = [(-49):(-41)] * pi / 180;
angles2 = [41:49] * pi / 180;

lineParams1 = FindWireAnglesAndRadii(binImg1, angles1, radiusSeparation, angleSeparation, numLines);
lineParams1 = sortrows(lineParams1, 2);
lineParams2 = FindWireAnglesAndRadii(binImg2, angles2, radiusSeparation, angleSeparation, numLines);
lineParams2 = sortrows(lineParams2, 2);

lineGap = ceil(radiusSeparation / 2);

% We now define a "window" in the *line coordinates* of each of the seven
% segments.  The window selects only the pixels whose radius and /lateral/
% coordinate fall inside it.  This way, we isolate the pixels on each of
% the segments into a single bucket.
img1LinesLoRadii = lineParams1(:,2) - lineGap;
img1LinesHiRadii = lineParams1(:,2) + lineGap;

img1LinesLoLaterals = [lineParams2(1, 2) + lineGap; lineParams2(2, 2) + lineGap; ...
        lineParams2(2, 2) + lineGap; lineParams2(2, 2) + lineGap; lineParams2(1, 2) + lineGap];
img1LinesHiLaterals = [lineParams2(5, 2) - lineGap; lineParams2(4, 2) - lineGap; ...
        lineParams2(4, 2) - lineGap; lineParams2(4, 2) - lineGap; lineParams2(5, 2) - lineGap];

img2LinesLoRadii = lineParams2(:,2) - lineGap;
img2LinesHiRadii = lineParams2(:,2) + lineGap;

img2LinesLoLaterals = [lineParams1(1, 2) + lineGap; lineParams1(2, 2) + lineGap; ...
        lineParams1(2, 2) + lineGap; lineParams1(2, 2) + lineGap; lineParams1(1, 2) + lineGap];
img2LinesHiLaterals = [lineParams1(5, 2) - lineGap; lineParams1(4, 2) - lineGap; ...
        lineParams1(4, 2) - lineGap; lineParams1(4, 2) - lineGap; lineParams1(5, 2) - lineGap];

% This is where the window is applied.  As we do this, we erase the pixels
% on the detected lines from the binary images.
for lineCounter = 1:5
    lineAngle = angles1( lineParams1(lineCounter, 1) );
    lineMatrix = [cos(lineAngle), sin(lineAngle); -sin(lineAngle), cos(lineAngle)];
    pixCoordsToLine = ( lineMatrix * [centeredPix1Cols'; centeredPix1Rows'] )';
    pixelXinLine = find( (pixCoordsToLine(:,1) >= img1LinesLoRadii(lineCounter)) ...
        .* (pixCoordsToLine(:,1) <= img1LinesHiRadii(lineCounter)) );
    pixCoordsToLine = pixCoordsToLine(pixelXinLine, :);
    pixelYinLine = find( (pixCoordsToLine(:,2) >= img1LinesLoLaterals(lineCounter)) ...
        .* (pixCoordsToLine(:,2) <= img1LinesHiLaterals(lineCounter)) );
    pixCoordsToLine = pixCoordsToLine(pixelYinLine, :);

    linePixels = [centeredPix1Cols, centeredPix1Rows];
    linePixels(pixelXinLine,:);
    linePixels = ans(pixelYinLine,:);

    lines1Pixels{lineCounter} = linePixels;
    
    binImg1 = PutPixelsOnImage(binImg1, lines1Pixels{lineCounter}, [centerCol, centerRow], 0);
    
%     imshow(binImg1);  pause;
end


for lineCounter = 1:5
    lineAngle = angles2( lineParams2(lineCounter, 1) );
    lineMatrix = [cos(lineAngle), sin(lineAngle); -sin(lineAngle), cos(lineAngle)];
    pixCoordsToLine = ( lineMatrix * [centeredPix2Cols'; centeredPix2Rows'] )';
    pixelXinLine = find( (pixCoordsToLine(:,1) >= img2LinesLoRadii(lineCounter)) ...
        .* (pixCoordsToLine(:,1) <= img2LinesHiRadii(lineCounter)) );
    pixCoordsToLine = pixCoordsToLine(pixelXinLine, :);
    pixelYinLine = find( (pixCoordsToLine(:,2) >= img2LinesLoLaterals(lineCounter)) ...
        .* (pixCoordsToLine(:,2) <= img2LinesHiLaterals(lineCounter)) );
    pixCoordsToLine = pixCoordsToLine(pixelYinLine, :);

    linePixels = [centeredPix2Cols, centeredPix2Rows];
    linePixels(pixelXinLine,:);
    linePixels = ans(pixelYinLine,:);

    lines2Pixels{lineCounter} = linePixels;

    binImg2 = PutPixelsOnImage(binImg2, lines2Pixels{lineCounter}, [centerCol, centerRow], 0);
    
%     imshow(binImg2); pause;
end

%%%%%%%%%%%%%%%%%%
% Now find the lines that extend on the outside of the distal plate
%%%%%%%%%%%%%%%%%%

img1PixelsInd = find(binImg1);
[img1PixRows, img1PixCols] = ind2sub(size(img1), img1PixelsInd);
centeredPix1Rows = img1PixRows - centerRow;
centeredPix1Cols = img1PixCols - centerCol;

img2PixelsInd = find(binImg2);
[img2PixRows, img2PixCols] = ind2sub(size(img2), img2PixelsInd);
centeredPix2Rows = img2PixRows - centerRow;
centeredPix2Cols = img2PixCols - centerCol;

extLineParams1 = FindWireAnglesAndRadii(binImg1, angles1, radiusSeparation, angleSeparation, 1);
extLineParams2 = FindWireAnglesAndRadii(binImg2, angles2, radiusSeparation, angleSeparation, 1);

img1ExtLineLoRadii = extLineParams1(1,2) - lineGap;
img1ExtLineHiRadii = extLineParams1(1,2) + lineGap;

img1ExtLineLoAbsLateral = max(-lineParams2(1,2), lineParams2(5,2)) + lineGap;
img1ExtLineHiAbsLateral = imgRadius - 2 * lineGap;

img2ExtLineLoRadii = extLineParams2(1,2) - lineGap;
img2ExtLineHiRadii = extLineParams2(1,2) + lineGap;

img2ExtLineLoAbsLateral = max(-lineParams1(1,2), lineParams1(5,2)) + lineGap;
img2ExtLineHiAbsLateral = imgRadius - 2 * lineGap;

lineAngle = angles1( extLineParams1(1, 1) );
lineMatrix = [cos(lineAngle), sin(lineAngle); -sin(lineAngle), cos(lineAngle)];
pixCoordsToLine = ( lineMatrix * [centeredPix1Cols'; centeredPix1Rows'] )';
pixelXinLine = find( (pixCoordsToLine(:,1) >= img1ExtLineLoRadii) ...
    .* (pixCoordsToLine(:,1) <= img1ExtLineHiRadii));
pixCoordsToLine = pixCoordsToLine(pixelXinLine, :);
pixelYinLine = find( (abs(pixCoordsToLine(:,2)) >= img1ExtLineLoAbsLateral) ...
    .* (abs(pixCoordsToLine(:,2)) <= img1ExtLineHiAbsLateral) );
pixCoordsToLine = pixCoordsToLine(pixelYinLine, :);

linePixels = [centeredPix1Cols, centeredPix1Rows];
linePixels(pixelXinLine,:);
linePixels = ans(pixelYinLine,:);

lines1Pixels{6} = linePixels;

lineAngle = angles2( extLineParams2(1, 1) );
lineMatrix = [cos(lineAngle), sin(lineAngle); -sin(lineAngle), cos(lineAngle)];
pixCoordsToLine = ( lineMatrix * [centeredPix2Cols'; centeredPix2Rows'] )';
pixelXinLine = find( (pixCoordsToLine(:,1) >= img2ExtLineLoRadii) ...
    .* (pixCoordsToLine(:,1) <= img2ExtLineHiRadii));
pixCoordsToLine = pixCoordsToLine(pixelXinLine, :);
pixelYinLine = find( (abs(pixCoordsToLine(:,2)) >= img2ExtLineLoAbsLateral) ...
    .* (abs(pixCoordsToLine(:,2)) <= img2ExtLineHiAbsLateral) );
pixCoordsToLine = pixCoordsToLine(pixelYinLine, :);

linePixels = [centeredPix2Cols, centeredPix2Rows];
linePixels(pixelXinLine,:);
linePixels = ans(pixelYinLine,:);

lines2Pixels{6} = linePixels;


if (~showPixels)
    return
end

figure;

for lineCounter=1:6
    lineImg = PutPixelsOnImage(mat2gray(image), lines1Pixels{lineCounter}, [centerCol, centerRow], [1 0 0]);
    imshow(lineImg);
    pause;
end

for lineCounter=1:6
    lineImg = PutPixelsOnImage(mat2gray(image), lines2Pixels{lineCounter}, [centerCol, centerRow], [1 0 0]);
    imshow(lineImg);
    pause;
end
