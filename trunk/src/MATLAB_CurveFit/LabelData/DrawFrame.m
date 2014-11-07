function DrawFrame(fgps, fim, mpp, figIndex)

index = str2double(inputdlg('frame num please'));

figure(figIndex);

gpsHeader = ReadHeader(fgps);
imHeader = ReadHeader(fim);

gps = RPreadgpsindex(fgps, gpsHeader, index);
[fx fy fz] = FrameBoundariesWorldCoords(imHeader, gps, mpp);
h = plot3(fx, fy, fz, 'r');
delete(h);


end