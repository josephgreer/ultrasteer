%left click means label point
%middle click means not sure
%right click means past edge

close all;
clearvars -except points;
clc;
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 4/scan';
f = strcat(fname, '.b8');
fcol = fopen(f, 'r');
f = strcat(fname, '.gps1');
fgps = fopen(f, 'r');

header = ReadHeader(fcol);

mpp = 83;

gpsheader = ReadHeader(fgps);

start = size(points,1);
gps = [];
for i=1:start
    im = RPreadframe(fcol, header);
    gpsPrev = gps;
    gps = RPreadgps(fgps, gpsheader);
end
for i=start+1:header.nframes
    hold off;
    im = RPreadframe(fcol, header);
    gpsPrev = gps;
    gps = RPreadgps(fgps, gpsheader);
    %im = im/255;
    imshow(im,[0 255]); colormap(gray);
    title(i);
    
    if(i > 1 && points(i-1,3) > 0)
        predPoint = PredictPoint(gpsPrev, gps, mpp, header, points(i-1, [1 2]));
        hold on;
        scatter(predPoint(1), predPoint(2), 500, 'r');
    end
    [x y button] = ginput(1);
    if(button == 1)
        points(i,:) = [x y 1];
    elseif(button == 2)
        points(i, :)= [x y 0];
    else
        points(i,:) = [x y -1];
    end
    %colormap(map);
end