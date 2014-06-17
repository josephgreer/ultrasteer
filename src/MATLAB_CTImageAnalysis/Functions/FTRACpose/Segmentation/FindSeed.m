% $Id: FindSeed.m,v 1.1 2005/10/27 19:10:47 svikal Exp $

% //-----------------------------------------------------------------------------
% // function:	FindSeed
% // file:		FindSeed.m
% // author:	siddharth
% // date:		
% task: to see if the given point is very close to points in the list

% // input:	array of existing points, point to look for

% // output: Index of the item in the list

% // calling function: called while deleting a seed or deleting a BB

% // NOTE: 
% //-----------------------------------------------------------------------------
function [ToDeleteItemIndex] =FindSeed(FinalInformationDataSet, CGToFind)
% final data set: [region/seed-number CGx CGy Orientation OrientationValidity PartOfCluster] 
% note that the CGx is in image coordinate system, while plotting, x & y
% should be swapped
MinDistance = 3;
ToDeleteItemIndex = -1;
for iCG = 1:length(FinalInformationDataSet(:,1))
    CG = FinalInformationDataSet(iCG,1:2);
    CurrentDistance = sqrt(sum((CGToFind-CG).^2));
    if (CurrentDistance < MinDistance)
        ToDeleteItemIndex = iCG;
        MinDistance = CurrentDistance;
    end
end


% $Log: FindSeed.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%