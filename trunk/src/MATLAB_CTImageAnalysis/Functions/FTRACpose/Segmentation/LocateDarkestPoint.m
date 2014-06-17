function [MinIntensityPoint] = LocateDarkestPoint (I, GivenPoint, SearchSize)

    % define a region of interest for interpolation around the
    % projected point as center of the region
    ROI_StartX = round(GivenPoint(1,1) - SearchSize - 1); ROI_EndX = round(GivenPoint(1,1) + SearchSize + 1); 
    ROI_StartY = round(GivenPoint(1,2) - SearchSize - 1); ROI_EndY = round(GivenPoint(1,2) + SearchSize + 1);
    Iroi = double(I(ROI_StartX:ROI_EndX, ROI_StartY:ROI_EndY));
    IntensityArray = [];

    % New min-search algorithm that replaces the old one. Here we call
    % interp2 at one go, instead of calling it again and again in the same
    % neighbourhood (as done in Siddharth's). All variables, etc.. have
    % been retained with the same convention that Siddharth used.
    [iY_Index iX_Index] = meshgrid(GivenPoint(1,2)-SearchSize:0.25:GivenPoint(1,2)+SearchSize, GivenPoint(1,1)-SearchSize:0.25:GivenPoint(1,1)+SearchSize);
    iX_Index = reshape(iX_Index, prod(size(iX_Index)), 1);
    iY_Index = reshape(iY_Index, prod(size(iY_Index)), 1);
    IntensityArray = [interp2(Iroi,  (iY_Index-ROI_StartY+1), (iX_Index-ROI_StartX+1), 'cubic') iX_Index iY_Index];
    
    iMinIndex = find(IntensityArray(:,1) == min(IntensityArray(:,1)));
    MinIntensityPoint = IntensityArray(iMinIndex(1), 2:3);

        
% $Log: LocateDarkestPoint.m,v $
% Revision 1.7  2007/01/02 18:36:35  jain
% Corrected a small bug where x and y got flipped while making the list.
%
% Revision 1.6  2006/12/29 23:44:22  jain
% This search is faster by calling interp2 only once over the complete list of
% neighbourhood points from which the minima is chosen.
%
% Revision 1.5  2006/12/29 19:29:08  anton
% LocateDarkestPoint: Add variable earch area.  Fix bug in select ROI on image.
%
% Revision 1.4  2006/12/29 18:29:54  anton
% LocateDarkestPoint: Reduced search are to -3 + 3 to reduce computation time
%
%
