function [lineParameters] = FindWireAnglesAndRadii(image, ...
    angleRange, radiusSeparation, angleSeparation, numPeaks)
% function [lineParameters] = FindWireAnglesAndRadii(image, ...
%     angleRange, radiusSeparation, angleSeparation, numPeaks)
%
% \param image a binary image that contains pixels on lines
% \param angleRange an array of angles in radians close to the angles of
% the lines in the image
% \param radiusSeparation, angleSeparation the supposed separation in
% parameter space (comprised of angle-distance pairs) between distinct
% lines.  Bigger separation means farther away detected lines.
% \param numPeaks the number of lines to search for, which is the same as
% the number of peaks to search for in the Hough transform histogram.
%
% \output lineParameters an array of size numPeaks*2 where the first
% component of each row is the angle index, and the second is the signed
% distance of the line from the image center, in pixel units.

[radiiHistogram, minRadius, maxRadius] = LinesHoughTransform(image, angleRange);

lineParameters = zeros(numPeaks, 2);

% figure; imagesc(radiiHistogram); pause

for j=1:numPeaks
    [rowOfMaxBin, colOfMaxBin, radiiHistogram, maxBin] = ...
        EliminateHistogramMax(radiiHistogram, radiusSeparation, angleSeparation);
    
    % imagesc(radiiHistogram); % disp(maxBin); pause;
    
    lineParameters(j, 1) = colOfMaxBin;
    lineParameters(j, 2) = rowOfMaxBin + minRadius;
end


function [rowOfMaxBin, colOfMaxBin, newHistogram, maxBin] = EliminateHistogramMax(histogram, rowSeparation, columnSeparation)

numRows = size(histogram,1);
numColumns = size(histogram, 2);

maxBin = max(max(histogram));
maxBinIndex = find(histogram == maxBin, 1);
[rowOfMaxBin, colOfMaxBin] = ind2sub([numRows, numColumns], maxBinIndex);

row0 = max(rowOfMaxBin - rowSeparation, 1);
row1 = min(rowOfMaxBin + rowSeparation, numRows);
col0 = max(colOfMaxBin - columnSeparation, 1);
col1 = min(colOfMaxBin + columnSeparation, numColumns);
newHistogram = histogram;  
newHistogram(row0:row1, col0:col1) = 0;

