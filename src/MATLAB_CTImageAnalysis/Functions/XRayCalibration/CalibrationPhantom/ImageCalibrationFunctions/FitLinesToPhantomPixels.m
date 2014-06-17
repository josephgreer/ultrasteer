function [a, b] = FitLinesToPhantomPixels(linePixels)
% function [a, b] = FitLinesToPhantomPixels(linePixels)
%
% Fit line equations of the form y = a*x + b to the data in linePixels.
% The linePixels array has the x coordinates (image column minus image
% center) at the first column, and the y coordinates at the second column.
% Note that generally not all lines are of the assumed form (e.g., lines
% parallel to the Y axis are not), and that the function assumes the lines
% were extracted from an image of the calibration phantom (see
% FindPhantomLinesInGrayImage).

% linePixels{6} = [linePixels{6}; linePixels{7}];

for j=1:6
    theLinePixels = linePixels{j};
    numPixels = size(theLinePixels, 1);
    xMatrix = [theLinePixels(:,1), ones(numPixels, 1)];
    yVector = theLinePixels(:,2);
    lineCoefficients = xMatrix \ yVector;
    a(j) = lineCoefficients(1);
    b(j) = lineCoefficients(2);
end
