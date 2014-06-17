%$Id: DewarpImageFromMappedCoordinates.m,v 1.3 2006/07/15 18:18:02 gouthami Exp $
function [dewarpIm] = DewarpImageFromMappedCoordinates(coordFilename, im)
% function dewarpIm = DewarpImageFromMappedCoordinates(coordFilename, im)
% This functions dewarps the image from the already mapped coordinates. The
% mapped coordinates are stored in a file given by coordFilename and im is
% the input image to be dewarped. 

% Read the coordinates from the file
[Xcoords, Ycoords] = ReadMappedCoordsFrmFile(coordFilename);

% interpolate the image
[rows, cols] = size(im);
Xcoords = reshape(Xcoords, [rows, cols]);
Ycoords = reshape(Ycoords, [rows, cols]);

dewarpIm = interp2(im, Ycoords, Xcoords);
dewarpIm(isnan(dewarpIm)) = 0;

%$Log: DewarpImageFromMappedCoordinates.m,v $
%Revision 1.3  2006/07/15 18:18:02  gouthami
%Overriding second party add
%