% $Id: AddToExistingWindow.m,v 1.1 2005/10/27 19:10:47 svikal Exp $
% //-----------------------------------------------------------------------------
% // function:	AddToExistingWindow() 
% // file:		AddToExistingWindow.m
% // author:		siddharth
% // date:		
% // task:		adds the center and its corresponding segments and its vote
% strength to its matching window
% //				
% // parameter:  WindowsList, Index, Vote strength, segments, total
% segments
% // return value: new updated Windows list
% corresponding index in the list
% // NOTE:		
% //-----------------------------------------------------------------------------
function [lWindowsMappingList] = AddToExistingWindow(iWindowIndexInMappingList, lWindowsMappingList, dVoteStrength, iRegionLabel1, iRegionLabel2, nTotalSegments)

%           lCenterWindowList = [RegionStartCornerX RegionStartCornerY RegionEndCornerX RegionEndCornerY iMappingIndex]
%           lWindowMappingList = [VoteStrength NumberOfSegments SegmentIds; VoteStrength NumberOfSegments SegmentIds;...]

CurrentNode = lWindowsMappingList(iWindowIndexInMappingList, :);
% add the vote strength
BonusUnit = 5;

lWindowsMappingList(iWindowIndexInMappingList, 1) = lWindowsMappingList(iWindowIndexInMappingList, 1) + dVoteStrength + BonusUnit;

% see if its necessary to add segments to the list, only if they are not
% already present


NumberOfAlreadyPresentSegments = CurrentNode(1,2);

NewNumberOfSegments = NumberOfAlreadyPresentSegments;

bRegion1Found = false;

bRegion2Found = false;

ExistingSegmentIds = CurrentNode(1,3:3+nTotalSegments-1);

NewSegmentIds = ExistingSegmentIds;

for iSegmentCount = 1: NumberOfAlreadyPresentSegments
    
    CurrentExistingSegment = ExistingSegmentIds(1,iSegmentCount);
    
    if (iRegionLabel1 == CurrentExistingSegment)
        bRegion1Found = true;
    end
    
    if (iRegionLabel2 == CurrentExistingSegment)
        bRegion2Found = true;
    end
    
end

if (~bRegion1Found || ~bRegion2Found)
    
    if(~bRegion1Found)
        
       NewNumberOfSegments = NewNumberOfSegments + 1;
       
       NewSegmentIds(1, NewNumberOfSegments)= iRegionLabel1;
       
    end
    
    if(~bRegion2Found)
        
       NewNumberOfSegments = NewNumberOfSegments + 1;
       
       NewSegmentIds(1, NewNumberOfSegments)= iRegionLabel2;
       
    end
    
    
end

lWindowsMappingList( iWindowIndexInMappingList, 2) = NewNumberOfSegments;

lWindowsMappingList( iWindowIndexInMappingList, 3:3+nTotalSegments-1) = NewSegmentIds;

% $Log: AddToExistingWindow.m,v $
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%