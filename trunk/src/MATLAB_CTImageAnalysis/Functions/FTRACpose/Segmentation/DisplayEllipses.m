% $Id: DisplayEllipses.m,v 1.4 2005/11/28 19:45:14 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	DisplayEllipses() 
% // file:		DisplayEllipses.m
% // author:		siddharth
% // date:		
% // description:	display the results of fiducial ellipse segmentation overlaid on
% the region of interest/whole image and also attaches Left-click event listener to the
% image area displayed & to the points plotted
% 
% input:	
% ellipses' points array containing the information
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
function DisplayEllipses (lEllipse1Points, lEllipse2Points, bOnWholeImage, FiducialSegmentationROITranslationVector);

global bEllipse1Removed; global bEllipse2Removed;
% % plot the ellipses on the image
CurrentAxesHandle = gca;
hold on;


% Please note that results obtained after ellipse fitting are in the
% coordinate system of the MATLAB, not our coordinate system, this is just
% for display purposes, to get an equations for out purposes, we will feed
% it the coordinate system we want

if (~bEllipse1Removed)
if (length(lEllipse1Points) >= 5)
    if (bOnWholeImage)
        [ellipse_t el_eq] = best_fit_ellipse(lEllipse1Points(:,2)+ FiducialSegmentationROITranslationVector(1,2), lEllipse1Points(:,1)+ FiducialSegmentationROITranslationVector(1,1), CurrentAxesHandle, false);
        text(ellipse_t.X0_in, ellipse_t.Y0_in, 'Ellipse 1', 'color', 'c');
    else
        [ellipse_t el_eq] = best_fit_ellipse(lEllipse1Points(:,2), lEllipse1Points(:,1), CurrentAxesHandle, true);
    end
end
end

if(~bEllipse2Removed)
if (length(lEllipse2Points) >= 5)
    if(bOnWholeImage)
        [ellipse_t el_eq] = best_fit_ellipse(lEllipse2Points(:,2)+ FiducialSegmentationROITranslationVector(1,2), lEllipse2Points(:,1)+ FiducialSegmentationROITranslationVector(1,1), CurrentAxesHandle, false);
        text(ellipse_t.X0_in, ellipse_t.Y0_in, 'Ellipse 2', 'color', 'c');
    else
        [ellipse_t el_eq] = best_fit_ellipse(lEllipse2Points(:,2), lEllipse2Points(:,1), CurrentAxesHandle, true);
    end
end
end

hold off;


% $Log: DisplayEllipses.m,v $
% Revision 1.4  2005/11/28 19:45:14  anton
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
% Revision 1.3  2005/11/07 00:05:11  svikal
% integrating with FTRAC;output of segmentation in desired format of FTRAC
%
% Revision 1.2  2005/11/05 22:56:12  svikal
% improved ellipse detection; changed the name of the function of fitting ellipse; hence file changed; also comments
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%