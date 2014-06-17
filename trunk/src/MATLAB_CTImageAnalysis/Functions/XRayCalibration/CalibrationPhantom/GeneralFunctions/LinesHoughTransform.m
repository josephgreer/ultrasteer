function [radiiHistMatrix, minRadius, maxRadius] = LinesHoughTransform(image, angles)
% function [radiiHistMatrix, minRadius, maxRadius] = LinesHoughTransform(image, angles)
% \param image a binary image that was filtered so that pixels along lines
% in the correct directions are 1, and the other pixels are 0.
% \param angles an array of angles in radians.  The function searches for
% lines at these angles.
%
% \output radiiHistMatrix a matrix counting the number of pixels incident
% on each line at the specified angles.  Each column corresponds to an
% angle in the list.  Each row corresponds to a radius in pixels, going
% from minRadius to maxRadius, inclusive.
% \output minRadius the smallest signed distance of a line from the center.
% Typically, it can be a negative number of a large magnitude.
% \output maxRadius the largest signed distance of a line from the center.
[imageRows, imageCols] = size(image);

centerRow = (imageRows + 1) / 2;
centerCol = (imageCols + 1) / 2;

pixelInd = find(image);
numPixels = length(pixelInd);
[pixelRows, pixelCols] = ind2sub( [imageRows, imageCols], pixelInd );
pixelRows = pixelRows - centerRow;
pixelCols = pixelCols - centerCol;

numAngles = prod(size(angles));
angles = reshape(angles, numAngles, 1);

% This matrix tranforms the pair [x; y] to a scalar l, which is
% the length of the projection of [x; y] on [cos(angle); sin(angle)],
% i.e., the distance of a line with of the given angle from the origin.
lineParametrizationMatrix = [cos(angles), sin(angles)];

% Each column in the result corresponds to an angle, and each column
% element to a pixel.  The value of the element is the radius
lineParameters = (lineParametrizationMatrix * [pixelCols'; pixelRows'])';

% round the radii (in pixel units) to the nearest integer
roundedRadii = round( lineParameters );

% Compute the number of different radii
minRadius = min(min(roundedRadii));
maxRadius = max(max(roundedRadii));
numRadii = maxRadius - minRadius + 1;

% Modulate the radii by the angle index as follows:
% 1. Subtract minRadius from all radii
% 2. Create a modulation matrix, which is added to the shifted radii.
shiftedRadii = roundedRadii - minRadius;
radiusModulations = (0:(numAngles-1)) * numRadii;
radiusModMatrix = ones( numPixels, 1 ) * radiusModulations;
modulatedRadiiMatrix = shiftedRadii + radiusModMatrix;
modulatedRadiiVector = reshape( modulatedRadiiMatrix, numPixels * numAngles, 1 );

% Compute histogram of the modulated radii.  Note that because radii at
% different angles are modulated, the same radius at different angles will
% be put in different bins
loBin = 0;
hiBin = numRadii * numAngles - 1;
radiiHistVector = hist( modulatedRadiiVector, loBin:hiBin );

% Wrap the bins back to a pattern where the columns are angles and the rows
% are radii.
radiiHistMatrix = reshape( radiiHistVector, numRadii, numAngles );

