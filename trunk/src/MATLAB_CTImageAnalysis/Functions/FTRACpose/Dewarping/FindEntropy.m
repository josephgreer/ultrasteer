%$Id: FindEntropy.m,v 1.1 2005/12/07 22:35:28 gouthami Exp $
function entropy = FindEntropy(centers, theta)
% function entropy = FindEntropy(centers, theta)
% FindEntropy calculated the entropy for given points 'centers' at given orientation
% 'theta'. Findentropy projects all the centers onto a horizontal axis and
% calculates the entropy based on the histogram probability of the
% projected points

% change theta from degrees to radians
theta = theta *pi/180;

% project the pts
projectedPts = centers(:,1)*cos(theta) - centers(:,2)*sin(theta);
numPts = size(projectedPts,1);
% create bins
binSize = 3;
numBins = round((max(projectedPts) - min(projectedPts))/binSize);
[numPtsInBin, binCenters] = hist(projectedPts, numBins);

entropy = 0;
% calculate probabilities
for i = 1:numPts
    pt = projectedPts(i);
    leftBinCenter = max(binCenters(binCenters<=pt));
    if isempty(leftBinCenter)
        leftBinCenter = binCenters(1);
    end
    leftBinIndex = find(binCenters == leftBinCenter);
    rightBinIndex = min(numBins, leftBinIndex + 1);
    rightBinCenter = binCenters(rightBinIndex);
    
    distFromLeftBin = abs(pt - leftBinCenter);
    distFromRightBin = abs(rightBinCenter - pt);
    
    if distFromLeftBin < distFromRightBin
        prob(i) = numPtsInBin(leftBinIndex)/numPts;
    else
        prob(i) = numPtsInBin(rightBinIndex)/numPts;
    end
     entropy = entropy +  prob(i) * log2(prob(i));
end

% $Log: FindEntropy.m,v $
% Revision 1.1  2005/12/07 22:35:28  gouthami
% Adding the file to CVS for the first time
%


