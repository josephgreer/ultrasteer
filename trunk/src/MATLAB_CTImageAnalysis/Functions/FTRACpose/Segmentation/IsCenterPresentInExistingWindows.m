% $Id: IsCenterPresentInExistingWindows.m,v 1.4 2007/01/08 19:55:55 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	IsCenterPresentInExistingWindows() 
% // file:		IsCenterPresentInExistingWindows.m
% // author:		siddharth
% // date:		
% // task:		finds out whether the given point lies within a certain
% window of pixels which has already been defined
% //				
% // parameter:  center point, Long_Axis, Short_Axis, Windows list
% // return value: flag to indicate whether its already present,If found
% corresponding index in the list
% // NOTE:		
% //-----------------------------------------------------------------------------
function [bAlreadyPresent, iWindowIndexInMappingList] = IsCenterPresentInExistingWindows(CurrentEllipseCenter, Current_Long_Axis, Current_Short_Axis, lCenterWindowList)

%           lCenterWindowList = [RegionStartCornerX RegionStartCornerY RegionEndCornerX RegionEndCornerY iMappingIndex]
%           lWindowMappingList = [VoteStrength NumberOfSegments SegmentIds; VoteStrength NumberOfSegments SegmentIds;...]


bAlreadyPresent = false;
iWindowIndexInMappingList = 0;
if (length(lCenterWindowList) > 1)   
    for iNodeCount = 1: length(lCenterWindowList(:,1))       
        CurrentNode = lCenterWindowList(iNodeCount, :);       
        CurrentNodeRegionStartCornerX = CurrentNode(1,1);      
        CurrentNodeRegionStartCornerY = CurrentNode(1,2);      
        CurrentNodeRegionEndCornerX = CurrentNode(1,3);   
        CurrentNodeRegionEndCornerY = CurrentNode(1,4);   
        CurrentNodeLongAxis = CurrentNode(1,5);    
        CurrentNodeShortAxis = CurrentNode(1,6);   
        CurrentNodeMappingIndex = CurrentNode(1,7);   
        if( (CurrentEllipseCenter(1,1) >= CurrentNodeRegionStartCornerX) && (CurrentEllipseCenter(1,1) <= CurrentNodeRegionEndCornerX) && (CurrentEllipseCenter(1,2) >= CurrentNodeRegionStartCornerY) && (CurrentEllipseCenter(1,2) <= CurrentNodeRegionEndCornerY) && (abs (CurrentNodeLongAxis - Current_Long_Axis) <= 10 ) && (abs (CurrentNodeShortAxis - Current_Short_Axis) <= 10 ))              
            bAlreadyPresent = true;      
            iWindowIndexInMappingList = CurrentNodeMappingIndex;
            break;
        end      
    end 
else
    return;
end


% $Log: IsCenterPresentInExistingWindows.m,v $
% Revision 1.4  2007/01/08 19:55:55  anton
% Segmentation: Tab and removed white lines
%
% Revision 1.3  2005/11/08 00:59:22  svikal
% relaxed/improved constraints on ellipse detection
%
% Revision 1.2  2005/11/05 22:56:12  svikal
% improved ellipse detection; changed the name of the function of fitting ellipse; hence file changed; also comments
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
