%$Id: FindCorrespondences.m,v 1.3 2006/09/27 15:03:45 gouthami Exp $
function [segPts, gridPts] = FindCorrespondences(centers, Origin, theta, res)
% function [segPts, gridPts] = FindCorrespondences(centers, Origin, theta)
% FindCorrespondences finds correspondences between the segmented points and the true
% grid points. FindCorrespondences assumes that the center BB has zero distortion, i.e, the center
% BB is mapped to itself after distortion correction.

numPts = length(centers);
% rotate the segmented points
rotatedCenters(:,1) = centers(:,1)*cos(theta) - centers(:,2)*sin(theta);
rotatedCenters(:,2) = centers(:,1)*sin(theta) + centers(:,2)*cos(theta);

% translate the segmented points
temp = ones(size(centers));
org(:,1) = Origin(1)*cos(theta) - Origin(2)*sin(theta);
org(:,2) = Origin(1)*sin(theta) + Origin(2)*cos(theta);
temp(:,1) = temp(:,1) * org(1);
temp(:,2) = temp(:,2) * org(2);
newCenters = rotatedCenters - temp;

% convert the center coordinates from pixels to mm
centers(:,1) = newCenters(:,1)*res(1);
centers(:,2) = newCenters(:,2)*res(2);

% find the correspondences
% fix the center BB with zero distortion
cc(1,:) = [0,0,0,0];
originIndex = find(centers(:,1)==0 & centers(:,2)==0);
centers(originIndex,:) = [];
numCorrPts = 1;

while numCorrPts < numPts
    prevNumCorrs = numCorrPts;
    for i=1:numCorrPts
        [cor,centers] = LabelGridPointsAroundOrigin(centers, cc(i,1:2), cc(i,3:4), 1);
        if (cor(1,1) ~= 1000)
            numNewCorrPts = size(cor,1);
            cc(end+1:end+numNewCorrPts,:) = cor;
        end
    end
   
    numCorrPts = size(cc,1);
    if (numCorrPts == prevNumCorrs)
        disp('Not all correspondences are established, check your data and threshold values in findcorrespondences.m file');
        break;
    end
end

% convert the mm to pixels
temp = ones(size(cc,1), 2);
temp(:,1) = temp(:,1) * org(1);
temp(:,2) = temp(:,2) * org(2);

segPts = cc(:,1:2);
s1 = [segPts(:,1)/res(1), segPts(:,2)/res(2)];
s2 = s1 + temp;
segPts(:,1) = s2(:,1)*cos(theta) + s2(:,2)*sin(theta);
segPts(:,2) = -s2(:,1)*sin(theta) + s2(:,2)*cos(theta);

gridPts = cc(:,3:4);
g1 = [gridPts(:,1)/res(1), gridPts(:,2)/res(2)];
g2 = g1 + temp;
gridPts(:,1) = g2(:,1)*cos(theta) + g2(:,2)*sin(theta);
gridPts(:,2) = -g2(:,1)*sin(theta) + g2(:,2)*cos(theta);

%$Log: FindCorrespondences.m,v $
%Revision 1.3  2006/09/27 15:03:45  gouthami
%FindCorrespondences: Modified the exit(termination) condition. This function returns the segmented
%BB positions for which the correspondences are established.
%
%Revision 1.2  2006/07/15 18:19:36  gouthami
%Added pixelRes as an input parameter to handle magnified images
%
%Revision 1.1  2005/12/07 22:37:38  gouthami
%Adding the file to CVS for the first time
%