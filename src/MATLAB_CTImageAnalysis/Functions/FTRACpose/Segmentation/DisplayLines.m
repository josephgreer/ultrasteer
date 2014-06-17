% $Id: DisplayLines.m,v 1.2 2005/11/28 19:45:14 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	DisplayLines() 
% // file:		DisplayLines.m
% // author:		siddharth
% // date:		
% // description:	display the results of fiducial lines segmentation overlaid on
% the region of interest/whole image and also attaches Left-click event listener to the
% image area displayed & to the points plotted
% 
% input:	
% line equations, x, y dimension bounds
% flag indicating whether on ROI or whole image
% variables that identify the region of interest coordinates
%           FiducialSegmentationROITranslationVector;
% 
% output: none
% 
% // calling function: DisplayFiducialSegmentationROI,
% DisplayFiducialSegmentationWholeImage
% // NOTE: 
% //-----------------------------------------------------------------------------
function DisplayLines(lLine1PointsEq, lLine2PointsEq, lLine3PointsEq, iX_Dimension, iY_Dimension, bOnWholeImage, FiducialSegmentationROITranslationVector);

global bLine1Removed; global bLine2Removed; global bLine3Removed;

    CurrentAxesHandle = gca;
    hold on;
 
    if(~bLine1Removed)
%   line 1  
    TextX = 1;
    TextY = 1;
    
    Y = 1:iY_Dimension;
    if (lLine1PointsEq(2) ~= 0)
        X = 1:iX_Dimension;
        Y = -((lLine1PointsEq(1)*X) + lLine1PointsEq(3))/lLine1PointsEq(2);
        TextY = 2;
        TextX = (-((lLine1PointsEq(1)*TextY) + lLine1PointsEq(3))/lLine1PointsEq(2)) - 10;
    else
        X = 1:iX_Dimension;
        X = -lLine1PointsEq(3)/lLine1PointsEq(1);
%         TextY = 5;
%         TextX = -lLine1PointsEq(3) - 10;
    end
     
    if (bOnWholeImage)
        plot( Y + FiducialSegmentationROITranslationVector(1,2), X + FiducialSegmentationROITranslationVector(1,1), 'y-');
        text(TextX + FiducialSegmentationROITranslationVector(1,2), TextY + FiducialSegmentationROITranslationVector(1,1), 'Line 1', 'color', 'y');
    else
        plot( Y , X, 'y-','ButtonDownFcn', 'ImageAxesClick_Callback');
    end
    
    end
    
    
    if(~bLine2Removed)
%   line 2    
    TextX = 1;
    TextY = 1;
    
    Y = 1:iY_Dimension;
    if (lLine2PointsEq(2) ~= 0)
        X = 1:iX_Dimension;
        Y = -((lLine2PointsEq(1)*X) + lLine2PointsEq(3))/lLine2PointsEq(2);
        TextY = 2;
        TextX = (-((lLine2PointsEq(1)*TextY) + lLine2PointsEq(3))/lLine2PointsEq(2)) - 10;
    else
        X = 1:iX_Dimension;
        X = -lLine2PointsEq(3)/lLine2PointsEq(1);
    end
     
    if (bOnWholeImage)
        plot( Y + FiducialSegmentationROITranslationVector(1,2), X + FiducialSegmentationROITranslationVector(1,1), 'y-');
        text(TextX + FiducialSegmentationROITranslationVector(1,2), TextY + FiducialSegmentationROITranslationVector(1,1), 'Line 2', 'color', 'y');
    else
        plot( Y , X, 'y-','ButtonDownFcn', 'ImageAxesClick_Callback');
    end

    end
    
    if(~bLine3Removed)
%   line 3
    TextX = 1;
    TextY = 1;
    
    Y = 1:iY_Dimension;
    if (lLine3PointsEq(2) ~= 0)
        X = 1:iX_Dimension;
        Y = -((lLine3PointsEq(1)*X) + lLine3PointsEq(3))/lLine3PointsEq(2);
        TextY = 2;
        TextX = (-((lLine3PointsEq(1)*TextY) + lLine3PointsEq(3))/lLine3PointsEq(2)) - 10;
    
    else
        X = 1:iX_Dimension;
        X = -lLine3PointsEq(3)/lLine3PointsEq(1);
    end
     
    if (bOnWholeImage)
        plot( Y + FiducialSegmentationROITranslationVector(1,2), X + FiducialSegmentationROITranslationVector(1,1), 'y-');
        text(TextX + FiducialSegmentationROITranslationVector(1,2), TextY + FiducialSegmentationROITranslationVector(1,1), 'Line 3', 'color', 'y');
    else
        plot( Y , X, 'y-','ButtonDownFcn', 'ImageAxesClick_Callback');
    end

    end
    
    hold off;
    
    
    % $Log: DisplayLines.m,v $
    % Revision 1.2  2005/11/28 19:45:14  anton
    % General Update from Siddharth:
    % *: Handling failure cases in fiducial segmentation, when only partial region
    %    is selected as region of interest for fiducial...however, note that the
    %    region should not be too small
    % *: Now the BB numbers, line numbers and ellipse numbers are displayed after
    %    establishing correspondence to have a visual check for user that
    %    correspondence established is correct; Note tht these are displayed only
    %    on whole image, not zoomed up region of interest, because there user can
    %    make changes....so just on whole image after segmentation & after user
    %    says he accepts the results
    % *: Few cases prev. unhandled in user intervention in fiducial segmentation
    %    validation
    %
    % Revision 1.1  2005/10/27 19:10:47  svikal
    % Added CVS tags
    %