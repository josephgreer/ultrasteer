close all;
clc;
fname = 'Y:/NeedleScan/Jan16_Scan1/Scan 6/scan';
f = strcat(fname, '.b8');
fcol = fopen(f, 'r');
f = strcat(fname, '.gps1');
fgps = fopen(f, 'r');

header = ReadHeader(fcol);

gpsheader = ReadHeader(fgps);

gps1 = RPreadgps(fgps, gpsheader);
for i=2:10
    gps2 = RPreadgps(fgps, gpsheader);
end

mpp = 83;

[ f1x f1y f1z ] = FrameBoundariesWorldCoords(header, gps1, mpp);
[ f2x f2y f2z ] = FrameBoundariesWorldCoords(header, gps2, mpp);

% plot3(f1x, f1y, f1z);
% hold on;
% plot3(f2x, f2y, f2z);

ul = header.ul;
br = header.br;
ur = [br(1) ul(2)];
bl = [ul(1) br(2)];

ul1 = MapImageCoordToWorldCoord(ul, header, mpp, gps1);
bl1 = MapImageCoordToWorldCoord(bl, header, mpp, gps1);
br1 = MapImageCoordToWorldCoord(br, header, mpp, gps1);
ur1 = MapImageCoordToWorldCoord(ur, header, mpp, gps1);

ul2 = MapImageCoordToWorldCoord(ul, header, mpp, gps2);
bl2 = MapImageCoordToWorldCoord(bl, header, mpp, gps2);
br2 = MapImageCoordToWorldCoord(br, header, mpp, gps2);
ur2 = MapImageCoordToWorldCoord(ur, header, mpp, gps2);

PlotPlane(ul1, ur1-ul1, bl1-ul1);
hold on;
PlotPlane(ul2, ur2-ul2, bl2-ul2);
%plot3(f2x, f2y, f2z);

n = cross(ur1-ul1, bl1-ul1);
n = (n/norm(n))*norm(ur1-ul1);
nn = [transpose((ul1+br1)/2); transpose((ul1+br1)/2)+transpose(n)];
plot3(nn(:,1), nn(:,2), nn(:,3));

intersection = PredictPoint(gps1, gps2, mpp, header, (ul+br)/2);
intersection = MapImageCoordToWorldCoord(intersection, header, mpp, gps2);
scatter3(intersection(1), intersection(2), intersection(3),'kx');


