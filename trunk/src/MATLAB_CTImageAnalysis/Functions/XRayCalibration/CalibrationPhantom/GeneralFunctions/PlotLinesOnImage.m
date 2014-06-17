function PlotLinesOnImage(imgSize, a, b, lineStyle)
% function PlotLinesOnImage(imgSize, a, b, lineStyle)
%
% This function plots color lines of the equations y = a*x + b , for all
% pairs (a,b) in the input parameters.  The line parameters are passed in
% with respect to a coordinate system whose origin is at the center of the
% image (computed from imgSize) and the units are image pixels.  However,
% this function computes the intersection points of these lines with the
% image boundaris, where these boudaries are in conventional matlab
% coordinates, that is: the top-left pixel is (1,1), the x coordinate is
% the column increasing to the right, and the y coordinate is the row
% increasing downwards.  Therefore, if a figure showing an image is held
% before this function is called, the lines will be plotted on it in the
% correct coordinates.

imgRows = imgSize(1);
imgCols = imgSize(2);

centerRow = (imgRows + 1) / 2;
centerCol = (imgCols + 1) / 2;

numLines = length(a);
a = reshape(a, 1, numLines);
b = reshape(b, 1, numLines);

b = b - a * centerCol + centerRow;



yAtXEq1 = a * 1 + b;
yAtXEqMax = a * imgCols + b;
xAtYEq1 = (1 - b) ./ a;
xAtYEqMax = (imgRows - b) ./ a;

xEnds = [max( xAtYEq1, ones(1, numLines) ); min( xAtYEqMax, imgCols * ones(1, numLines) )];
yEnds = [a; a] .* xEnds + [b; b];

line(xEnds, yEnds, 'LineStyle', lineStyle);
