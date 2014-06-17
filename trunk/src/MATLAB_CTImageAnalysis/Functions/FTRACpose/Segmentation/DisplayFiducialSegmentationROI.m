% $Id: DisplayFiducialSegmentationROI.m,v 1.2 2006/04/20 20:38:58 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	DisplayFiducialSegmentationROI() 
% // file:		DisplayFiducialSegmentationROI.m
% // author:		siddharth
% // date:		
% // description:	display the results of fiducial segmentation overlaid on
% the region of interest and also attaches Left-click event listener to the
% image area displayed & to the points plotted
% 
% // assumed input:	
% global variable I (image buffer)
% variables that identify the region of interest coordinates
%           global FiducialROIStartX; 
%           global FiducialROIStartY; 
%           global FiducialROIXWidth; 
%           global FiducialROIYWidth;
%           global FiducialSegmentationROITranslationVector;
% variables which are floating buffers for the results, which are modified
% when user performs Add or Delete operations for BBs, Lines, Ellipses, 
% all the changes are made to these buffers rather than original, 
% it is copied onto original when user clicks Accept button
%             global Floating_BBLocations;
%             global Floating_lLine1PointsEq;
%             global Floating_lLine2PointsEq;
%             global Floating_lLine3PointsEq; 
%             global Floating_lEllipse1Points;
%             global Floating_lEllipse2Points;         
% variable that identifies the current image handle
%             global CurrentImageHandle;
% 
% output: none
% 
% // calling function: called when Select region button is pressed, after
% performing segmentation,  after user performs some
% action i.e some change, results need to be visually updated, then this
% function is called.
% // NOTE: 
% //-----------------------------------------------------------------------------

function DisplayFiducialSegmentationROI()
    % global variable required:
    global I;

    global Floating_BBLocations;
    global Floating_lLine1PointsEq; global Floating_lLine2PointsEq; global Floating_lLine3PointsEq; 
    global Floating_lEllipse1Points; global Floating_lEllipse2Points;
    global FiducialROIStartX; global FiducialROIStartY; global FiducialROIXWidth; global FiducialROIYWidth;

    iX_Dimension = FiducialROIXWidth + 1;
    iY_Dimension = FiducialROIYWidth + 1;

    cla;
    % refresh the image
    CurrentImageHandle = imshow(I(FiducialROIStartX:FiducialROIStartX + FiducialROIXWidth, FiducialROIStartY:FiducialROIStartY + FiducialROIYWidth));
    set(CurrentImageHandle, 'ButtonDownFcn', 'ImageAxesClick_Callback');
    bOnWholeImage = false;

    DisplayBB(Floating_BBLocations, bOnWholeImage, 0);
    DisplayLines(Floating_lLine1PointsEq, Floating_lLine2PointsEq, Floating_lLine3PointsEq, iX_Dimension, iY_Dimension, bOnWholeImage, 0);
    DisplayEllipses (Floating_lEllipse1Points, Floating_lEllipse2Points, bOnWholeImage, 0);


% $Log: DisplayFiducialSegmentationROI.m,v $
% Revision 1.2  2006/04/20 20:38:58  anton
% Segmentation: DisplayFiducialSegmentationROI, tabify and format.
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%