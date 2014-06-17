function mask = CreateEllipsoidMask(imageSize, minCoords, maxCoords)

ellipsoidCenter = (minCoords + maxCoords) / 2;
ellipsoidRadii = (maxCoords - minCoords) / 3;

rows = imageSize(1);
cols = imageSize(2);
slcs = imageSize(3);

[maskXCoord, maskYCoord, maskZCoord] = meshgrid([(-ellipsoidCenter(2)):(-ellipsoidCenter(2) + cols - 1)], ...
    [(-ellipsoidCenter(1)):(-ellipsoidCenter(1) + rows - 1)], ...
    [(-ellipsoidCenter(3)):(-ellipsoidCenter(3) + slcs - 1)]);

mask = (maskXCoord / ellipsoidRadii(2)) .^ 2 + (maskYCoord / ellipsoidRadii(1)) .^ 2 + ...
    (maskZCoord / ellipsoidRadii(3)) .^ 2 < 1;
