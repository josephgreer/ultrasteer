function range = FindGPSPosRange(fgps, header)
xrange = [1e6 -1e6]; yrange = xrange; zrange = xrange;
for i=1:header.nframes
    cGps = RPreadgpsindex(fgps, header, i);
    
    xrange = [min(xrange(1),cGps.pos(1)) max(xrange(2),cGps.pos(1))];
    yrange = [min(yrange(1),cGps.pos(2)) max(yrange(2),cGps.pos(2))];
    zrange = [min(zrange(1),cGps.pos(3)) max(zrange(2),cGps.pos(3))];
end
xrange = [xrange(1)-50 xrange(2)+50];
yrange = [yrange(1)-50 yrange(2)+50];
zrange = [zrange(1)-50 zrange(2)+50];
range = [xrange; yrange; zrange];
end