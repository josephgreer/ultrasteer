%$Id: ExtractLines.m,v 1.3 2007/07/18 20:27:11 anton Exp $

function labelIm = ExtractLines(calibIm, gridPts, threshVal, resRatioToNTSC)
% function labelIm = ExtractLines(calibIm, gridPts) extracts the lines in
% the calibration/dewarp image by removing the BBs. The BB positions are
% given in 'gridPts' variable
bb_diameter = 2 * resRatioToNTSC; % multiplied by resolution
for i = 1:size(gridPts, 1)
    x = [round(gridPts(i,1) - bb_diameter): round(gridPts(i,1) + bb_diameter)];
    y = [round(gridPts(i,2) - bb_diameter): round(gridPts(i,2) + bb_diameter)];
    x1 = x(x >0);
    y1 = y(y>0);
    calibIm(x1,y1) = 0.6;
end

threshIm = calibIm <threshVal;
threshIm = imclearborder(threshIm);
labelIm = bwlabel(threshIm);

% identify the lines as the longest connected components
avgNumPixelsInLine = 150 * resRatioToNTSC * resRatioToNTSC; % multiplied by resolution ratio square, this is a surface
for i = 1:max(max(labelIm))
    counts = length(find(labelIm == i));
    if (counts <= avgNumPixelsInLine)
        labelIm(labelIm==i) = 0;
    end
end
labelIm = bwlabel(labelIm);

%$Log: ExtractLines.m,v $
%Revision 1.3  2007/07/18 20:27:11  anton
%Dewarping: Added resolution ratio to NTSC to handle different resolutions.
%All sizes (for segmentation) remain in pixels for NTSC resolution as this has
%been our reference so far (i.e. about 9 inch diameter for 480 pixels).
%This should ultimately be modified to define all sizes in mm and use a more
%intuitive definition of the resolution.
%
%Revision 1.2  2006/09/21 11:09:55  gouthami
%removed the hard-coded threshold value
%
%Revision 1.1  2006/07/15 18:13:52  gouthami
%Adding to CVS
%
