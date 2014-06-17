%input:
%e is the distance from the point to the plane containing the circle
%f is the distance from the point to the cyclinder containing the circle
%cutoff is the cuttoff (number of standard deviations away from the mean)
%at which data is removed (culled) from the pool
% Orignal Author Tom Wedlick, Modified by Ann Majewicz

function [curvePointsCulled] = removeOutliers(curvePoints,d, cutoff)  %Other outputs ,avgDist, stdDev. Don't think it's necessary for what I need. AM 10/1/10

[a,b]= size(curvePoints);

avgDist = mean(d);

stdDev = std(d);

%Remove all od the points that are cuttoff standard deviations away from the mean
indicies = ((d- avgDist)>= cutoff*stdDev) == 0;
curvePointsCulled = curvePoints(indicies,:);
