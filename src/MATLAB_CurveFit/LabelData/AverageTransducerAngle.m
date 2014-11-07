tvals = TvalsFromDescriptor(mps(:,[1:3]), descriptor);

nframes = max(mps(:,7));
angles = [];

deriv = [];
for i=degree:-1:1
    deriv = [deriv i*model(:,degree-i+1)];
end
    

for i=1:nframes
    [pts] = find(mps(:,7)==i);
    if(size(pts,1) == 0)
        continue;
    end
    gps = RPreadgpsindex(fgps, gpsheader, i);
    tval = tvals(pts(1));
    tval = tval.^[degree-1:-1:0]';
    tangent = deriv*tval;
    tangent = tangent/norm(tangent);
    
    [fbsx fbsy fbsz] = FrameBoundariesWorldCoords(gpsheader, gps, mpp);
    fbs = [fbsx fbsy fbsz];
    
    xx = fbs(2,:)-fbs(1,:);
    yy = fbs(4,:)-fbs(1,:);
    
    
    normal = cross(xx,yy);
    normal = normal/norm(normal);
    
    angle = acos((normal*tangent));
    %angle = abs(angle);
    %angle = mod(angle+2*pi, pi/2);
    angles = [angles; angle];
end

aveAngle = mean(angles)*180/pi;