% $Id: DisplayBB.m,v 1.3 2005/11/28 19:45:14 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	DisplayBB() 
% // file:		DisplayBB.m
% // author:		siddharth
% // date:		
% // description:	display the results of fiducial BB segmentation overlaid on
% the region of interest/whole image and also attaches Left-click event listener to the
% image area displayed & to the points plotted
% 
% input:	
% BBLocations array containing the information
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
function DisplayBB(BBLocations, bOnWholeImage, FiducialSegmentationROITranslationVector);

% % plot BB locations on image
 CurrentAxesHandle = gca;
 hold on;
% 
 for iBB = 1: length(BBLocations)
     if (BBLocations (iBB,1) ~= -1)
         if (bOnWholeImage)
             plot(BBLocations(iBB,2) + FiducialSegmentationROITranslationVector(1,2), BBLocations(iBB,1) + FiducialSegmentationROITranslationVector(1,1), 'g.');
             text (BBLocations(iBB,2) + FiducialSegmentationROITranslationVector(1,2) - 10, BBLocations(iBB,1) + FiducialSegmentationROITranslationVector(1,1), num2str(iBB), 'color', 'g');
         else
             plot(BBLocations(iBB,2), BBLocations(iBB,1), 'g.' ,'ButtonDownFcn', 'ImageAxesClick_Callback');
         end
     end
 end
 
 hold off;

 
 % $Log: DisplayBB.m,v $
 % Revision 1.3  2005/11/28 19:45:14  anton
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
 % Revision 1.2  2005/11/03 15:55:08  svikal
 % changes made for establishing correspondence of BBs
 %
 % Revision 1.1  2005/10/27 19:10:47  svikal
 % Added CVS tags
 %