function [vertLinesPixels, horzLinesPixels, vertRadii, horzRadii] = ...
    FindWireGridLinesInGrayImage(image, convFilterSize, captureRegionBinaryMask, captureThresh, ...
    numLines, captureAngles, radiusSeparation, angleSeparation, ...
    showPixels, pauseCallbackHandle)
% function [vertLinesPixels, horzLinesPixels, vertRadii, horzRadii] = ...
%     FindWireGridLinesInGrayImage(image, convFilterSize, captureThresh, ...
%     captureAngles, captureRegion, numLines, radiusSeparation, angleSeparation, ...
%     showPixels, pauseCallbackHandle)
%
% This function finds the pixels on vertical and horizontal lines in a dark
% pattern on bright backround.  The lines are found first through
% template-matching convolution, and then through Hough transform with some
% tolerance around the angles.
%
% \param image a grayscale image that contains the pattern as dark lines on
% bright background.  Eg., and x-ray image.
%
% \param convFilterSize the size of convolution filter to find candidates
% for line pixels.  See the code for more information.
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
% \param captureAngles a set of angles for the lines that we're after. 0 is
% parallel to the image Y axis (row increasing); positive rotation is
% clockwise.
%
% \param numLines the number of veritcal and horizontal lines to search in
% the image.
%
% \param radiusSeparation A number specifying the separation between
% peaks in the Hough transform of the line pattern, which detects the
% individual lines.  Given in pixel units.  See FindWireAnglesAndRadii.
% Half of this value is set as a "line gap safety zone":  Once a line's
% radius and angle parameters were decided, only the pixels along a line of
% the found angle at a distance of radiusSeparation/2 from the central
% radius are counted towards the line.  This reduces chances of a "false
% positive" detection of lines.
%
% \param angleSeparation A number specifying separation between peaks in
% the Hough transform in the angle dimension.  Typically, this number
% should be the number of angles specified for the range.
%
% \param showPixels if true then the program displayes each of the
% detected features.
%
% \param pauseCallbackHandle a function handle.  If showPixels is true,
% this function is called to wait for user response before switching to the
% next view of output.  By default, this handle is equal to @pause .
%
% \output vertLinesPixels an object array where each element is an array of
% pixel coordinates with respect to the image center, which are detected as
% being on one of the vertical lines.  The positive X direction is from
% left to right.
% \output horzLinesPixels an object array where each element is an array of
% pixel coordinates on the horizontal lines.  The positive Y direction is
% from top to bottom.
% \output vertRadii the *signed* radii, that is, estimated distances, of
% the vertical lines from the image center, in pixel units.
% \output horzRadii the signed estimated of the horizontal lines from the
% image center, in pixel units.


[numRows, numCols] = size(image);
imgRadius = min(numRows / 2, numCols / 2);
centerRow = (numRows + 1) / 2;
centerCol = (numCols + 1) / 2;

maskVert = VertWireConvFilter(convFilterSize, 1);
maskHorz = maskVert';  % HorzWireConvFilter(maskSize, 1);

if (isempty(captureRegionBinaryMask) || isscalar(captureRegionBinaryMask))
    captureRegionBinaryMask = ones(size(image));
end

%img1 = conv2(image, maskVert, 'samesize');
img1 = conv2(single(image),single(maskVert), 'samesize');
localMaxImg1 = [zeros(numRows, 2), ( (img1(:,3:(numCols-2)) > img1(:,1:(numCols-4))) & ...
    (img1(:,3:(numCols-2)) > img1(:,5:numCols)) ), zeros(numRows, 2)];
maxVert = max(max(img1));
threshVert = maxVert * captureThresh;
binImg1 = (img1 > threshVert) .* localMaxImg1 .* captureRegionBinaryMask;

%img2 = conv2(image, maskHorz, 'samesize');
img2 = conv2(single(image), single(maskHorz), 'samesize');
localMaxImg2 = [zeros(2, numCols); ( (img2(3:(numRows-2),:) > img2(1:(numRows-4),:)) & ...
    (img2(3:(numRows-2),:) > img2(5:numRows,:)) ); zeros(2, numCols)];
maxHorz = max(max(img2));
threshHorz = maxHorz * captureThresh;
binImg2 = (img2 > threshHorz) .* localMaxImg2 .* captureRegionBinaryMask;

% binIntersections = binImg1 .* binImg2;
% binImg1 = binIntersections;
% binImg2 = binIntersections;


img1PixelsInd = find(binImg1);
[img1PixRows, img1PixCols] = ind2sub(size(img1), img1PixelsInd);
centeredPix1Rows = img1PixRows - centerRow;
centeredPix1Cols = img1PixCols - centerCol;

img2PixelsInd = find(binImg2);
[img2PixRows, img2PixCols] = ind2sub(size(img2), img2PixelsInd);
centeredPix2Rows = img2PixRows - centerRow;
centeredPix2Cols = img2PixCols - centerCol;

% The Hough transform parametrizes lines by their normal angle and radius,
% i.e., distance from the origin.  It counts how many pixels are incident
% on each such line.  We select 5 peaks for each direction, which represent
% the 7 segments, three of which are nearly collinear.
anglesVert = captureAngles * pi / 180;
anglesHorz = (captureAngles + 90) * pi / 180;

lineParamsVert = FindWireAnglesAndRadii(binImg1, anglesVert, ...
     radiusSeparation, angleSeparation, numLines);
lineParamsVert = sortrows(lineParamsVert, 2);

lineParamsHorz = FindWireAnglesAndRadii(binImg2, anglesHorz, ...
     radiusSeparation, angleSeparation, numLines);
lineParamsHorz = sortrows(lineParamsHorz, 2);

vertRadii = lineParamsVert(:,2);
horzRadii = lineParamsHorz(:,2);
lineGap = ceil(radiusSeparation / 2);

% We now define a "window" in the *line coordinates* of each of the seven
% segments.  The window selects only the pixels whose radius and /lateral/
% coordinate fall inside it.  This way, we isolate the pixels on each of
% the segments into a single bucket.
vertLinesLoRadii = lineParamsVert(:,2) - lineGap;
vertLinesHiRadii = lineParamsVert(:,2) + lineGap;

horzLinesLoRadii = lineParamsHorz(:,2) - lineGap;
horzLinesHiRadii = lineParamsHorz(:,2) + lineGap;


% This is where the window is applied.
for lineCounter = 1:numLines
    lineAngle = anglesVert( lineParamsVert(lineCounter, 1) );
    lineMatrix = [cos(lineAngle), sin(lineAngle)];
    pixCoordsToLine = ( lineMatrix * [centeredPix1Cols'; centeredPix1Rows'] )';

    pixelCoordInBounds = (pixCoordsToLine >= vertLinesLoRadii(lineCounter)) .* ...
        (pixCoordsToLine <= vertLinesHiRadii(lineCounter));
    pixelInBounds = sum(pixelCoordInBounds, 2);
    pixelXinLine = find( pixelInBounds );

    linePixels = [centeredPix1Cols, centeredPix1Rows];
    linePixels = linePixels(pixelXinLine,:);

    vertLinesPixels{lineCounter} = linePixels;
    
    % Avoid re-use of pixels already found
    centeredPix1Cols(pixelXinLine) = [];
    centeredPix1Rows(pixelXinLine) = [];
end


for lineCounter = 1:numLines
    lineAngle = anglesHorz( lineParamsHorz(lineCounter, 1) );
    lineMatrix = [cos(lineAngle), sin(lineAngle)];
    pixCoordsToLine = ( lineMatrix * [centeredPix2Cols'; centeredPix2Rows'] )';
    
    pixelCoordInBounds = (pixCoordsToLine >= horzLinesLoRadii(lineCounter)) .* ...
        (pixCoordsToLine <= horzLinesHiRadii(lineCounter));
    pixelInBounds = sum(pixelCoordInBounds, 2);
    pixelYinLine = find( pixelInBounds );
    
    linePixels = [centeredPix2Cols, centeredPix2Rows];
    linePixels = linePixels(pixelYinLine,:);

    horzLinesPixels{lineCounter} = linePixels;  
    % Avoid re-use of pixels already found
    centeredPix2Cols(pixelYinLine) = [];
    centeredPix2Rows(pixelYinLine) = [];
end


if (~showPixels)
    return
end

if (~exist('pauseCallbackHandle'))
    pauseCallbackHandle = @pause;
end

colorTable = [1, 0, 0; ...
    0, 1, 0; ...
    0, 0, 1; ...
    0, 1, 1; ...
    1, 0, 1; ...
    1, 1, 0; ...
    ];
numColors = size(colorTable, 1);
hfig = figure;

lineImg = mat2gray(image);
for lineCounter = 1:numLines
    linePixels = vertLinesPixels{lineCounter};
    colorIndex = mod(lineCounter - 1, numColors) + 1;
    lineImg = PutPixelsOnImage(lineImg, linePixels, [centerCol, centerRow], colorTable(colorIndex,:));
end

imshow(lineImg);
pauseCallbackHandle();
lineImg = mat2gray(image);

for lineCounter = 1:numLines
    linePixels = horzLinesPixels{lineCounter};
    colorIndex = mod(lineCounter - 1, numColors) + 1;
    lineImg = PutPixelsOnImage(lineImg, linePixels, [centerCol, centerRow], colorTable(colorIndex,:));
end

imshow(lineImg);
pauseCallbackHandle();
close(hfig);
