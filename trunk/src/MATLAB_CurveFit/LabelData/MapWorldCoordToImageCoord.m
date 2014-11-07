%%%%% Assumes point is given in world coordinates [x y z]
%%%%% gps is GPS structure containing position and orientation information 
%%%%% of frame.  
%%%%% return value is image coordinates of point.  

function pos = MapWorldCoordToImageCoord(world, header, mpp, gps)
ul = [320 0];
gps_cal = [14.8449	 0.9477	 -0.0018; 15.0061	0.0016	1.00; 0.1638	0.0166	0.0052];
image = inv(gps.pose*gps_cal)*(world-gps.pos);
if(abs(image(1)-1) > .05)
    error('invalid image coord');
end
%image coord now contains [1 y x]
pos = [image(3) image(2)];
pos = pos*1000/mpp;
pos = pos+ul;
end