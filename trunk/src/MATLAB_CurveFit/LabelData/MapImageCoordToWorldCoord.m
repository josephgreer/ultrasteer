%%%%% Assumes point is image coordinate given by [x y];
%%%%% gps is GPS structure containing position and orientation information 
%%%%% of frame.  
%%%%% return value is world coordinates of point.

function pos = MapImageCoordToWorldCoord(point, header, mpp, gps)
if(header.ld == 0 || header.extra == 0)
    ul = [320 0];
else
    ul = [header.ld 0];% header.extra];
end
if(header.dr == 0)
    mpp = 83;
else
    mpp = header.dr;
end

point = point-ul;
point = [point(1)*mpp/1000 point(2)*mpp/1000];
gps_cal = [14.8449	 0.9477	 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052];

pt = [1; point(2); point(1)];
pos = gps.pose*gps_cal*pt+gps.pos;
end