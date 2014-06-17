function mask = CreateOvalMask(imageSizeRC, xRange, yRange)
% function mask = CreateOvalMask(imageSizeRC, xRange, yRange)
%
% Create a rectangular binary mask with an oval region set to 1.
%
% \param imageSizeRC : [rows, cols] vector specifying the mask size
% \param xRange : [xmin, xmax] vector of range of oval in the X (column)
%   direction.
% \param yRange : [ymin, ymax] vector of range of oval in the Y (row)
%   direction.
maskXCenter = (xRange(2) + xRange(1)) / 2;
maskXRadius = (xRange(2) - xRange(1)) / 2;
maskYCenter = (yRange(2) + yRange(1)) / 2;
maskYRadius = (yRange(2) - yRange(1)) / 2;

[maskXCoord, maskYCoord] = meshgrid(1:imageSizeRC(2), 1:imageSizeRC(1));
maskXCoord = (maskXCoord - maskXCenter);
maskYCoord = (maskYCoord - maskYCenter);
maskXCoord = maskXCoord / maskXRadius;
maskYCoord = maskYCoord / maskYRadius;

distanceSquare = (maskXCoord .^ 2) + (maskYCoord .^ 2);

mask = distanceSquare <= 1;


