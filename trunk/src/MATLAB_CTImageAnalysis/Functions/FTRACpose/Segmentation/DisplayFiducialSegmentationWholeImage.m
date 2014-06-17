% $Id: DisplayFiducialSegmentationWholeImage.m,v 1.2 2006/04/20 20:40:34 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	DisplayFiducialSegmentationWholeImage() 
% // file:		DisplayFiducialSegmentationWholeImage.m
% // author:		siddharth
% // date:		
% // description:	display the results of fiducial segmentation overlaid on
% the whole image

% // assumed input:	
% variables that identify the region of interest coordinates
%           global FiducialROIXWidth; 
%           global FiducialROIYWidth;
%           global FiducialSegmentationROITranslationVector;
% variables for the results, 
%             global BLocations;
%             global lLine1PointsEq;
%             global lLine2PointsEq;
%             global lLine3PointsEq; 
%             global lEllipse1Points;
%             global lEllipse2Points;         
% 
% output: none
% 
% // calling function: called when Select region button is pressed, after
% performing segmentation,  after the 'Accept' button is pressed
% // NOTE: 
% //-----------------------------------------------------------------------------
function DisplayFiducialSegmentationWholeImage()
    global rufSegmentationData;
    if (rufSegmentationData.FiducialSegmentationPerformed == true)
        % global variable required:
        global FiducialSegmentationROITranslationVector;
        global BBLocations;
        global lLine1PointsEq;global lLine2PointsEq;global lLine3PointsEq; 
        global lEllipse1Points;global lEllipse2Points; 
        global FiducialROIXWidth; global FiducialROIYWidth;

        CurrentAxesHandle = gca;
        hold on;

        iX_Dimension = FiducialROIXWidth + 1;
        iY_Dimension = FiducialROIYWidth + 1;
        bOnWholeImage = true;

        DisplayBB(BBLocations, bOnWholeImage, FiducialSegmentationROITranslationVector);
        DisplayLines(lLine1PointsEq, lLine2PointsEq, lLine3PointsEq, iX_Dimension, iY_Dimension, bOnWholeImage, FiducialSegmentationROITranslationVector);
        DisplayEllipses (lEllipse1Points, lEllipse2Points, bOnWholeImage, FiducialSegmentationROITranslationVector);
        
        hold off;
    end

% $Log: DisplayFiducialSegmentationWholeImage.m,v $
% Revision 1.2  2006/04/20 20:40:34  anton
% Segmentation: DisplayXyzSegmentationWholeImage, use global flag to
% determine if should draw or not, tabify/format.
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
