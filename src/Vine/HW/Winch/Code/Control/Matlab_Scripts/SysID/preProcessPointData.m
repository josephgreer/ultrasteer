function [xs R0, plane] = preProcessPointData(gpsHeader,fgps,positionFrames,verticalOffset,radius)
xs = {};

firstPoint = RPreadgpsindex(fgps, gpsHeader, positionFrames(1));
firstPoint.pos = firstPoint.pos-firstPoint.pose(:,3)*radius;

swapper = [0 0 1;...
           0 -1 0;...
           1 0 0];

for i=1:length(positionFrames)
    cGps = RPreadgpsindex(fgps, gpsHeader, positionFrames(i));
    
    x.pos = cGps.pos-firstPoint.pos+firstPoint.pose(:,1)*verticalOffset;
    x.pos = x.pos-cGps.pose(:,3)*radius;
    cGps.pose = cGps.pose*swapper;
    x.q = RotationMatrixToQuat(cGps.pose);
    xs{i} = x;
end

R0 = firstPoint.pose*swapper;

plane = Plane([R0(:,3); -dot(R0(:,3),xs{1}.pos)]);

end