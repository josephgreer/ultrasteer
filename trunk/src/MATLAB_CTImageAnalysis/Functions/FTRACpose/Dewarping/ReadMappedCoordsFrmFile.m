%$Id: ReadMappedCoordsFrmFile.m,v 1.1 2006/03/07 20:33:58 gouthami Exp $
function [Xmapped, Ymapped] = ReadMappedCoordsFrmFile(filename)

% function [Xmapped, Ymapped] = ReadMappedCoordsFrmFile(filename)
% reads the mapped coordinates from the given file


fid = fopen(filename, 'rt');
nPixels = fscanf(fid, '%d', 1);
pixelCoords = fscanf(fid, '%f', nPixels *2);

pixelCoords = reshape(pixelCoords, [2, nPixels]);
pixelCoords = pixelCoords';

Xmapped = pixelCoords(:,1);
Ymapped = pixelCoords(:,2);

fclose(fid);

