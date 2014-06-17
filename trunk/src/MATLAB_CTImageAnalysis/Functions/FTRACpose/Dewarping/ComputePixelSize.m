% $Id: ComputePixelSize.m,v 1.2 2006/09/22 18:45:04 gouthami Exp $
function pixelSize = ComputePixelSize(bbPositions, origin, theta)

% function pixelSize = ComputePixelSize(bbPositions, origin) computes the
% pixel size of the input image based on the bbPositions.

originIndex = find(bbPositions(:,1) == origin(1) & bbPositions(:,2) == origin(2));
affineRotMatrix = [cos(theta) -sin(theta); sin(theta) cos(theta)] ;
bbPositions = (affineRotMatrix * bbPositions')' ;

origin = bbPositions(originIndex, :);

bbPositions(originIndex, :) = [];
xDelta = bbPositions(:,2) - origin(2);
xIndices = find(abs(xDelta) < 5);
ptsOnXaxis = bbPositions(xIndices, :);
dist = ptsOnXaxis - repmat(origin, [size(xIndices), 1]);
dist = dist.*dist; dist = dist(:,1) + dist(:,2); dist = sqrt(dist);
nearestIndex = find(dist == min(dist));
XMMPerPixel = 10/abs(origin(1) - ptsOnXaxis(nearestIndex, 1));

yDelta = bbPositions(:,1) - origin(1);
yIndices = find(abs(yDelta) < 5);
ptsOnYaxis = bbPositions(yIndices, :);
dist = ptsOnYaxis - repmat(origin, [size(yIndices), 1]);
dist = dist.*dist; dist = dist(:,1) + dist(:,2); dist = sqrt(dist);
nearestIndex = find(dist == min(dist));
YMMPerPixel = 10/abs(origin(2) - ptsOnYaxis(nearestIndex, 2));

pixelSize = [XMMPerPixel, YMMPerPixel];

%$Log: ComputePixelSize.m,v $
%Revision 1.2  2006/09/22 18:45:04  gouthami
%ComputePixelSize: This function now considers orientation to determine pixel sizes
%
%Revision 1.1  2006/09/19 00:30:29  gouthami
%Adding to CVS
%