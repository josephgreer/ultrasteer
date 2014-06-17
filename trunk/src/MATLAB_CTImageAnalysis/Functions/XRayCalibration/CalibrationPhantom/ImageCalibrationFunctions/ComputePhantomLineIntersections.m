function [intersectionX, intersectionY] = ComputePhantomLineIntersections(lineParams1, lineParams2)
% function [intersectionX, intersectionY] = ComputePhantomLineIntersections(lineParams1, lineParams2)
%
% This function computes line intersection given line parameters (a,b) of
% the form y = a*x + b.  The line parameters are typically output of the
% function FitLinesToPhantomPixels.  lineParams1 refers to lines whose
% angle is -45 degrees, and lineParams2 refers to lines whose angle is +45
% degrees.  The order of the intersections returned corresponds exactly to
% the order of model feature points against which the calibration is
% computed.  It is the same order as in LoadCalibrationPatternGeometry .

% interior corners - on the proximal plane
[x1, y1] = IntersectLines( lineParams1(2,1), lineParams1(2,2), lineParams2(2,1), lineParams2(2,2) );
[x2, y2] = IntersectLines( lineParams1(2,1), lineParams1(2,2), lineParams2(4,1), lineParams2(4,2) );
[x3, y3] = IntersectLines( lineParams1(4,1), lineParams1(4,2), lineParams2(2,1), lineParams2(2,2) );
[x4, y4] = IntersectLines( lineParams1(4,1), lineParams1(4,2), lineParams2(4,1), lineParams2(4,2) );

% interior midpoints - on the proximal plane
[x5, y5] = IntersectLines( lineParams1(3,1), lineParams1(3,2), lineParams2(2,1), lineParams2(2,2) );
[x6, y6] = IntersectLines( lineParams1(3,1), lineParams1(3,2), lineParams2(4,1), lineParams2(4,2) );
[x7, y7] = IntersectLines( lineParams1(2,1), lineParams1(2,2), lineParams2(3,1), lineParams2(3,2) );
[x8, y8] = IntersectLines( lineParams1(4,1), lineParams1(4,2), lineParams2(3,1), lineParams2(3,2) );

% center point on proximal plate
[x9, y9] = IntersectLines( lineParams1(3,1), lineParams1(3,2), lineParams2(3,1), lineParams2(3,2) );

% exterior corners - on the distal plane
[x10, y10] = IntersectLines( lineParams1(1,1), lineParams1(1,2), lineParams2(1,1), lineParams2(1,2) );
[x11, y11] = IntersectLines( lineParams1(1,1), lineParams1(1,2), lineParams2(5,1), lineParams2(5,2) );
[x12, y12] = IntersectLines( lineParams1(5,1), lineParams1(5,2), lineParams2(1,1), lineParams2(1,2) );
[x13, y13] = IntersectLines( lineParams1(5,1), lineParams1(5,2), lineParams2(5,1), lineParams2(5,2) );

% exterior midpoints - on the distal plane
[x14, y14] = IntersectLines( lineParams1(6,1), lineParams1(6,2), lineParams2(1,1), lineParams2(1,2) );
[x15, y15] = IntersectLines( lineParams1(6,1), lineParams1(6,2), lineParams2(5,1), lineParams2(5,2) );
[x16, y16] = IntersectLines( lineParams1(1,1), lineParams1(1,2), lineParams2(6,1), lineParams2(6,2) );
[x17, y17] = IntersectLines( lineParams1(5,1), lineParams1(5,2), lineParams2(6,1), lineParams2(6,2) );

% center point on distal plate
[x18, y18] = IntersectLines( lineParams1(6,1), lineParams1(6,2), lineParams2(6,1), lineParams2(6,2) );

intersectionX = [x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14 x15 x16 x17 x18]';
intersectionY = [y1 y2 y3 y4 y5 y6 y7 y8 y9 y10 y11 y12 y13 y14 y15 y16 y17 y18]';

function [x, y] = IntersectLines(a1, b1, a2, b2)
x = (b2 - b1) / (a1 - a2);
y = a1 * x + b1;