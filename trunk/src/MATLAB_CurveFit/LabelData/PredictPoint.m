%%%%%% This function takes a point in frame x and maps it to frame x+1
%%%%%% using frame x's plane normal.
%%%%%% i.e. it intersects a line going through a point in frame x with
%%%%%% direction to the frame normal of frame x with image plane of frame
%%%%%% x+1
function pos = PredictPoint(gps_1, gps_2, mpp, frameHeader, point, dir)
ul = frameHeader.ul;
br = frameHeader.br;
ur = [br(1) ul(2)];
bl = [ul(1) br(2)];

ul1 = MapImageCoordToWorldCoord(ul, frameHeader, mpp, gps_1);
bl1 = MapImageCoordToWorldCoord(bl, frameHeader, mpp, gps_1);
ur1 = MapImageCoordToWorldCoord(ur, frameHeader, mpp, gps_1);

if nargin < 6
    n = cross(ur1-ul1, bl1-ul1);
    n = (n/norm(n));
else
    n = dir;
end

pointa = MapImageCoordToWorldCoord(point, frameHeader, mpp, gps_1);
pointb = pointa+n;


pt0 = MapImageCoordToWorldCoord(ul, frameHeader, mpp, gps_2); 
pt1 = MapImageCoordToWorldCoord(ur, frameHeader, mpp, gps_2);
pt2 = MapImageCoordToWorldCoord(bl, frameHeader, mpp, gps_2);

A = [pointa-pointb pt1-pt0 pt2-pt0];
b = pointa-pt0;
intersection = inv(A)*b;
pos = pointa+(pointb-pointa)*intersection(1);

pos = MapWorldCoordToImageCoord(pos, frameHeader, mpp, gps_2);
end