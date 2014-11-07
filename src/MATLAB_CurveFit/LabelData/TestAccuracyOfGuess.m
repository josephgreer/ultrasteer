function TestAccuracyOfGuess(fitPoints, fgps, fim, poly, descriptor, degree, mpp, indices)
index = str2double(inputdlg('frame index please'));

fheader = ReadHeader(fim);
gheader = ReadHeader(fgps);
im = RPreadframeindex(fim, fheader, index);
gps = RPreadgpsindex(fgps, gheader, index);

figure(20);
imshow(im, [0 255]);
pt = ginput(1);

pt = MapImageCoordToWorldCoord(pt, gheader, mpp, gps);

close 20;
ShowAccuracyOfPointIndex(transpose(pt), fitPoints, fgps, fim, poly, descriptor, degree, mpp, index, indices);
end