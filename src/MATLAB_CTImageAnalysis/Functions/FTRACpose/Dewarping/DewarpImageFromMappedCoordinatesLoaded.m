% $Id: DewarpImageFromMappedCoordinatesLoaded.m,v 1.1 2007/01/08 16:49:55 anton Exp $
function [dewarpIm] = DewarpImageFromMappedCoordinatesLoaded(Xcoords, Ycoords, im)
% function dewarpIm = DewarpImageFromMappedCoordinates(Xcoords, Ycoords, im)
% This functions dewarps the image from the already mapped coordinates. The
% mapped coordinates are stored in memory and im is
% the input image to be dewarped. 

% interpolate the image
[rows, cols] = size(im);
Xcoords = reshape(Xcoords, [rows, cols]);
Ycoords = reshape(Ycoords, [rows, cols]);

dewarpIm = interp2(im, Ycoords, Xcoords);
dewarpIm(isnan(dewarpIm)) = 0;

% $Log: DewarpImageFromMappedCoordinatesLoaded.m,v $
% Revision 1.1  2007/01/08 16:49:55  anton
% Dewarp: Added function to dewarp from already loaded coordinates to avoid
% loading the file over and over.
%
%
