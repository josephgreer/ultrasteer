%$Id: LabelGridPointsAroundOrigin.m,v 1.2 2006/03/07 20:36:04 gouthami Exp $


function [corr_centers, centers] = LabelGridPointsAroundOrigin(centers, origin, label, index)
% function [corr_centers, centers] = LabelGridPointsAroundOrigin(centers, origin, label, index)
% LabelGridPointsAroundOrigin labels the grid points on the x and y axis around
% origin at a distance of 10mm. LabelGridPointsAroundOrigin assume that the origin
% is already labelled. The labelling starts with the origin and grows
% outwards until all the points are labelled. To label the neighbouring
% points, LabelGridPointsAroundOrigin finds the four closest points to the origin
% and determines the label based on their orientation, i.e., if a point is
% at an angle of 85 degrees and deltay is +ve, then that point lies on the
% positive y-axis.
% 

% initialize centers to a maximum value
corr_centers = [1000, 1000, 1000, 1000];

% number of neighbours to look for
numNeighbours = 4;
% create a temporary array of origin values
temp = ones(size(centers));
temp(:,1) = temp(:,1) * origin(1);
temp(:,2) = temp(:,2) * origin(2);

% find the distance of all the centers to the origin
delta = centers - temp;
delta = delta.*delta ; 
dist = sqrt(delta(:,1)+delta(:,2));

% sort the distances
[Y, I] = sort(dist);

% take the first four minimas, i.e. four points closest to the origin
if (size(I)< numNeighbours)
    numNeighbours = size(I);
end
closest_pts = centers(I(1:numNeighbours), :);

% find the orientation
for i = 1:numNeighbours
    cpt = centers(I(i),:);
    dx= cpt(1) - origin(1);
    dy = cpt(2) - origin(2);
    ang = 90;
    if (dx ~= 0)
        ang = abs(atan(dy/dx)*180/pi); 
    end
    % if angle is between 75 and 90
    if (ang > 75)
        if (abs(dy) <15) %  threshold to handle the missing BB's
            if (dy >0) % +ve y-axis
                corr_centers(index,:) = [centers(I(i), :), label + [0, 10]];
                index = index+1;
            else  % -ve y-axis
                corr_centers(index,:) = [centers(I(i), :), label + [0, -10]];
                index = index+1;
            end
        end
    end
    % x - axis  ( angle between 0 and 30)
    if (ang >=0 && ang <=30)
        if (abs(dx) < 15) % threshold to handle missing BB's - can be improved
            if (dx >0) % +ve x-axis
                corr_centers(index,:) = [centers(I(i), :), label + [10, 0]];
                index = index+1;
            else % -ve x-axis
                corr_centers(index,:) = [centers(I(i), :), label + [-10, 0]];
                index = index +1;
            end
        end
    end
end

for i=1:size(corr_centers,1)
    index = find(centers(:,1)==corr_centers(i,1) & centers(:,2)==corr_centers(i,2));
    centers(index,:)=[];
end

%$Log: LabelGridPointsAroundOrigin.m,v $
%Revision 1.2  2006/03/07 20:36:04  gouthami
%Fixed the zero case when finding the orientation of the BB, which was causing the
%divide with zero warning
%
%Revision 1.1  2005/12/07 22:34:46  gouthami
%Adding the file to CVS for the first time
%