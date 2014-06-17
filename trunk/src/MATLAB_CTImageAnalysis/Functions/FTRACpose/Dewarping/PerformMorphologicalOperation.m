% $Id: PerformMorphologicalOperation.m,v 1.2 2007/07/18 20:27:11 anton Exp $
function resultIm = PerformMorphologicalOperation(im, resRatioToNTSC)
% function resultIm = PerformMorphologicalOperation(im) performs
% morphological top hat operation on the input image im.
% This code is borrowed from Seed Segmentation part written by Siddharth
% Vikal

% disp(' Performing the morphological top hat operation');
invIm = imcomplement(im);

% perform the morphological top hat operation
% a) perform opening with 3x3 square
% Iop = imopen(Ineg, strel([1 1 1; 1 1 1; 1 1 1]));
% a) perform opening with 5x5 rectangle
SE = strel('disk', round(7 * resRatioToNTSC)); % diameter was 7 for NTSC
Iop = imopen(invIm, SE);

% Iop = imopen(Ineg, strel([0 0 1 1 1; 0 0 1 1 1; 1 1 1 1 1; 1 1 1 0 0;1 1 1 0 0]));
% b) subtract from the original image to get the final gray scale image
resultIm = imsubtract(invIm, Iop);

% $Log: PerformMorphologicalOperation.m,v $
% Revision 1.2  2007/07/18 20:27:11  anton
% Dewarping: Added resolution ratio to NTSC to handle different resolutions.
% All sizes (for segmentation) remain in pixels for NTSC resolution as this has
% been our reference so far (i.e. about 9 inch diameter for 480 pixels).
% This should ultimately be modified to define all sizes in mm and use a more
% intuitive definition of the resolution.
%
% Revision 1.1  2006/09/19 00:22:53  gouthami
% Adding to CVS
%
