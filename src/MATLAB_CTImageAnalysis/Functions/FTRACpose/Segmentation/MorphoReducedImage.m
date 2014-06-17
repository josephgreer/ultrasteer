% $Id: MorphoReducedImage.m,v 1.3 2007/09/19 14:50:17 anton Exp $

% //-----------------------------------------------------------------------------
% // function:	MorphoReducedImage() 
% // file:		MorphoReducedImage.m
% // author:		siddharth
% // date:		
% // description:	reduce the fiducial image to an image conatining only
% ellipse & line segments; basically get rid of most of the noise; this is
% done by morphological top hat transform by taking a structuring which is
% a disk of radius 1; this is because our lines & ellipse are 2-3 pixels
% thick; this followed by area opening to get rid noise picked up in the
% process; we know we are looking for connected components

% // input:	I (image buffer) only of the region of interest

% // output: image with only lines and ellipse segments

% // calling function: ExtractEllipses()

% // NOTE: 
% //-----------------------------------------------------------------------------
function [IreducedImage] = MorphoReducedImage(I, resRatioToNTSC)

Ith = imtophat(I, strel('disk', ceil(1 * resRatioToNTSC)));

Ithbin = Ith > 3;

Iareaopen = bwareaopen(Ithbin, round(15 * resRatioToNTSC));

IreducedImage = Iareaopen;

% $Log: MorphoReducedImage.m,v $
% Revision 1.3  2007/09/19 14:50:17  anton
% Segmentation: Take Ratio to NTSC into account for ellipses
%
% Revision 1.2  2005/11/05 22:56:12  svikal
% improved ellipse detection; changed the name of the function of fitting ellipse; hence file changed; also comments
%
% Revision 1.1  2005/10/27 19:10:47  svikal
% Added CVS tags
%
