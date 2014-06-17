function mask = CreateOvalMask3D(imageSizeRCL, xRange, yRange, zRange)
% function mask = CreateOvalMask3D(imageSizeRCL, xRange, yRange, zRange)
%
% Create a box-shape binary mask with an ellipsoid region set to 1.
%
% \param imageSizeRCL : [rows, cols, layers] vector specifying the mask size
% \param xRange : [xmin, xmax] vector of range of oval in the X (column)
%   direction.
% \param yRange : [ymin, ymax] vector of range of oval in the Y (row)
%   direction.
% \param zRange : [zmin, zmax] vector of range of oval in the Z (layer)
%   direction.

maskXCenter = (xRange(2) + xRange(1)) / 2;
maskXRadius = (xRange(2) - xRange(1)) / 2;
maskYCenter = (yRange(2) + yRange(1)) / 2;
maskYRadius = (yRange(2) - yRange(1)) / 2;
maskZCenter = (zRange(2) + zRange(1)) / 2;
maskZRadius = (zRange(2) - zRange(1)) / 2;

[maskXCoord, maskYCoord, maskZCoord] = meshgrid(1:imageSizeRCL(2), 1:imageSizeRCL(1), 1:imageSizeRCL(3));
maskXCoord = (maskXCoord - maskXCenter) / maskXRadius;
maskYCoord = (maskYCoord - maskYCenter) / maskYRadius;
maskZCoord = (maskZCoord - maskZCenter) / maskZRadius;

mask = (maskXCoord .^ 2) + (maskYCoord .^ 2) + (maskZCoord .^ 2) <= 1;


