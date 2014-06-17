% $Id: ProcessImage.m,v 1.7 2007/07/21 00:13:48 anton Exp $
function [labelIm, Origin] = ProcessImage(im, thresh, resRatioToNTSC)
% function [labelIm, Origin] = ProcessImage(im)
% ProcessImage performs morphological
% segmentation to segement BB's. ProcessImage segments the BB along with
% the lines. These lines are removed from the image based on the length of
% the connected components given by BWLabel function. ProcessImage returns a 
% labelled image with only BB's along with the center of the central BB in the grid
%
% im - image of the calibration phantom with BB's and lines
% labelIm - labelled image with only BB's
% Origin - position of the center BB.
% Note: The morphological segmentation (initial parts) part of this
% function is borrowed from Siddharth Vikal.

% threshold the image & convert into binary, threshold values can be 40 -70
% disp(' thresholding the image');
threshIm = im > thresh;
threshIm = imclearborder(threshIm);


% remove the calibration lines from the image
labelIm = bwlabel(threshIm);
for i = 1:max(max(labelIm))
    c = length(find(labelIm ==i));
    if (c > round(150 * resRatioToNTSC * resRatioToNTSC))
        labelIm(labelIm==i) =0;
    end
    if (c < round(15 * resRatioToNTSC * resRatioToNTSC)) % surface use resolution square
        labelIm(labelIm == i) = 0;
    end
end
% find the axis and origin of the BB plate
labelIm = bwlabel(labelIm);
% disp(' finding the orign and axis of the BB plate');
axisIm = labelIm;
stats = regionprops(axisIm, 'area');
c = cat(1,stats.Area);
for i = 1:max(max(axisIm))
    if (c(i)<40)
        axisIm(axisIm==i) =0;
    end
end

axisIm = bwlabel(axisIm);

% find the origin - center of the BB closest to the image center
% disp(' finding the BB centers');
method = 2;
axisCenters = CalculateCentroid(im, axisIm, method);
imgCenter = size(im)/2;
minDist = 1000;
for i = 1:length(axisCenters)
    dist= norm(axisCenters(i,:) - imgCenter);
    if dist <minDist
        minDist = dist;
        Origin = axisCenters(i,:);
    end
end

% axisBBPositions = axisCenters(unique(axisIm(axisIm > 0)), :);


%$Log: ProcessImage.m,v $
%Revision 1.7  2007/07/21 00:13:48  anton
%Dewarping ProcessImage: Use square of resolution ratio to NTSC for surfaces
%
%Revision 1.6  2007/07/18 20:27:11  anton
%Dewarping: Added resolution ratio to NTSC to handle different resolutions.
%All sizes (for segmentation) remain in pixels for NTSC resolution as this has
%been our reference so far (i.e. about 9 inch diameter for 480 pixels).
%This should ultimately be modified to define all sizes in mm and use a more
%intuitive definition of the resolution.
%
%Revision 1.5  2006/09/27 15:05:06  gouthami
%ProcessImage: Changed the area of the large BB (on the axis) from 30 to 40. These are
%empirical values and can be sensitive to the input pixel size.
%
%Revision 1.4  2006/09/19 00:34:04  gouthami
%Separated morphological processing from this function
%
%Revision 1.3  2006/07/15 18:25:42  gouthami
%corrected cvs tags
%
%Revision 1.2  2006/07/15 18:24:50  gouthami
%added "thresh" value as a parameter
%
%Revision 1.1  2005/12/07 22:33:00  gouthami
%Adding the file to CVS for the first time
%
