function [lineDistance] = length2plane (startPoint, lineVector, planePoint, planeNormal)
% Accepts a start point and a vector in the direction that will be traveled
% and returns the distance along the supplied vector to reach the end point
% starting from the startPoint

% Calculate the shortest vector from the start point to the plane



%Calculate a vector from any point in the plane to the start point of the line;
vectLine2Plane = (planePoint - startPoint);

%Project that vector onto the normal to determine the distance from the
%line to the plane
D = dot(vectLine2Plane, planeNormal);

%Project this shortest distane along the lineVector to determine the
%distance along the line vector that is takes to intersect the specified
%plane

lineDistance = D*dot ( planeNormal, lineVector) / norm(lineVector);





