% $Id: DefineNewWindow.m,v 1.3 2007/01/08 19:55:32 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	DefineNewWindow() 
% // file:		DefineNewWindow.m
% // author:		siddharth
% // date:		
% // task:		defines a new window around the given center, adds it to
% windows list
% //				
% // parameter:  WindowsList, WindowMappingList, bounds of x,y dimensions
% of the image, center, Major_Axis, Minor_Axis, vote strength, segment ids, total segments
% // return value: new updated Windows list, Windowsmapping list
% corresponding index in the list
% // NOTE:		
% //-----------------------------------------------------------------------------
function [lCenterWindowList, lWindowsMappingList] = DefineNewWindow(CurrentEllipseCenter, Major_Axis, Minor_Axis, iX_Dimension, iY_Dimension,lCenterWindowList, lWindowsMappingList, dVoteStrength, i, j, nTotalSegments)

%           lCenterWindowList = [RegionStartCornerX RegionStartCornerY RegionEndCornerX RegionEndCornerY iMappingIndex]
%           lWindowMappingList = [VoteStrength NumberOfSegments SegmentIds; VoteStrength NumberOfSegments SegmentIds;...]
SegmentIds = zeros(1, nTotalSegments);
SegmentIds (1,1) = i;
SegmentIds (1,2) = j;
NodeToAddInWindowsMappingList = [dVoteStrength 2 SegmentIds];
lWindowsMappingList = [lWindowsMappingList; NodeToAddInWindowsMappingList];

if (length(lWindowsMappingList) > 1)
    iMappingIndex = length(lWindowsMappingList(:,1));
else
    iMappingIndex = 1;
end

[RegionStartCornerX RegionStartCornerY RegionEndCornerX RegionEndCornerY] = ComputeRegionEnclosingCenter(CurrentEllipseCenter, iX_Dimension, iY_Dimension);
NodeToAddInCenterWindowList = [RegionStartCornerX RegionStartCornerY RegionEndCornerX RegionEndCornerY Major_Axis Minor_Axis iMappingIndex];
lCenterWindowList = [lCenterWindowList; NodeToAddInCenterWindowList];


% $Log: DefineNewWindow.m,v $
% Revision 1.3  2007/01/08 19:55:32  anton
% Segmentation: Tab and removed white lines
%
% Revision 1.2  2005/11/05 22:56:12  svikal
% improved ellipse detection; changed the name of the function of fitting ellipse; hence file changed; also comments
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
