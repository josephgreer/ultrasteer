function [prm, linesA, linesB] = CalibrateFromImage(image, convMaskSize, ...
    radiusSeparation, angleSeparation, modelPoints, pixelSize, showPixels, ...
    showLines, lineStyle, showIntersections, noise)
% function [prm, linesA, linesB] = CalibrateFromImage(image, convMaskSize, ...
%     radiusSeparation, angleSeparation, modelPoints, pixelSize, showPixels, ...
%     showLines, lineStyle, showIntersections, noise)
%
% A complete calibration procedure that produces the intrinsic calibration
% parameters (source position in "detector" coordinates")
%
% \param image a grayscale fluoro image of the calibration grid
%
% \param convMaskSize the size of filter used to detect the diagonal lines in
% the image in pixel units.
%
% \param radiusSeparation, angleSeparation an estimate of the separation
% between line parameters in Hough space for the diagonal lines in the
% image.  Good numbers are 8 for radiusSeparation and 10 for
% angleSeparation.
%
% \param modelPoints a 18*3 array of coordinates of the intersections
% between phantom lines.  The coordinates should be obtained, e.g., from 
% LoadCalibrationPatternGeometry; modelPoints = calibrationPhantomPoints;
%
% \param pixelSize the size of image pixels in the X (column) and Y (row)
% directions.  This number is determined, for example, when the image is
% dewarped.
%
% \param showPixels boolean; if true the pixels of each detected diagonal
% segment will be displayed in a figure.
%
% \param showLines boolean; if true the computed lines that were matched to
% the detected pixels will be displayed in a figure.
%
% \param lineStyle character describing the line style for the lines
% displayed if showLines is true
%
% \param showIntersections boolean; if true the locations of line
% intersections will be displayed in a figure.
%
% \param noise data for simulating uncertainty in the detection of line
% intersections, for robustness tests.  Optional parameter, normally set to zero.
%
% \output prm a 3-element vector specifying the position of the x-ray
% source in virtual-detector coordinates.  The virtual detector's origin is
% at the dewarp grid center.
% \output linesA,linesB line parameters for the detected phantom lines.


[numRows, numCols] = size(image);
[lines1Pixels, lines2Pixels] = FindPhantomLinePixelsInGrayImage(image, convMaskSize, ...
    radiusSeparation, angleSeparation, showPixels);
[a_plus45, b_plus45] = FitLinesToPhantomPixels(lines1Pixels);
[a_minus45, b_minus45] = FitLinesToPhantomPixels(lines2Pixels);

if (showLines)
    figure;
    imshow(image, []);
    hold;
    PlotLinesOnImage([numRows, numCols], a_plus45, b_plus45, lineStyle);
    PlotLinesOnImage([numRows, numCols], a_minus45, b_minus45, lineStyle);
end

linesA = [a_plus45, a_minus45];
linesB = [b_plus45, b_minus45];

[intersectionX, intersectionY] = ComputePhantomLineIntersections([a_plus45', b_plus45'], [a_minus45', b_minus45']);

if exist('noise')
    intersectionX = intersectionX + 2 * noise * rand(size(intersectionX)) - noise;
    intersectionY = intersectionY + 2 * noise * rand(size(intersectionY)) - noise;
end

intersectionsPointsMm = [intersectionX * pixelSize(1), intersectionY * pixelSize(2)];

% We assume that the positive X direction in image pixels, that is,
% increasing column index, corresponds to to the positive X direction of
% the phantom, and likewise for the increasing image rows and the phantom's
% positive Y direction.

if (showIntersections)
    centerRow = (numRows + 1) / 2;
    centerCol = (numCols + 1) / 2;

    intersectionXPixel = intersectionX + centerCol;
    intersectionYPixel = intersectionY + centerRow;

    plot(intersectionXPixel, intersectionYPixel, 'ow');
end

prm = ComputeIntrinsicParametersCalibration(modelPoints, intersectionsPointsMm);
    