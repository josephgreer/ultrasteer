%left click means label point
%middle click means not sure
%right click means past edge

close all;
clearvars -except points gpoints;
clc;
fname = 'Y:/NeedleScan/11_26_14/Scan4/scan';
f = strcat(fname, '.b8');
fcol = fopen(f, 'r');
f = strcat(fname, '.gps1');
fgps = fopen(f, 'r');

header = ReadHeader(fcol);

mpp = header.dr;
origin = [header.ld header.extra];

gpsheader = ReadHeader(fgps);

if(~exist('points'))
    points  = [];
end

if(~exist('gpoints'))
    gpoints = [];
end

nskip = 5;
start = 1;
if(size(points,1) > 0)
    start = max(points(:,4)+nskip);
end
for i=start:nskip:header.nframes
    hold off;
    im = RPreadframeindex(fcol, header,i);
    gps = RPreadgpsindex(fgps, gpsheader,i);
    gpsPrev = gps;
    imshow(im,[0 255]); colormap(gray);
    title(i);
    
    [x y button] = ginput(1);
    globalPoint = MapImageCoordToWorldCoord([x y],header,mpp,gps);
    if(button == 1)
        points = [points;[x y 1 i]];
        gpoints = [gpoints; [globalPoint' 1 i]];
    elseif(button == 2)
        points = [points; [x y 0 i]];
        gpoints = [gpoints; [globalPoint' 0 i]];
    else
        points = [points; [x y -1 i]];
        gpoints = [gpoints; [globalPoint' -1 i]];
    end
end


figure;
scatter3(gpoints(:,1), gpoints(:,2), gpoints(:,3));
daspect([1 1 1]);
xlabel('x');
ylabel('y');
zlabel('z');