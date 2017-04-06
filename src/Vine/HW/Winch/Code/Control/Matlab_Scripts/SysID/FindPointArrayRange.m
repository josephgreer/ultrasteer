function range = FindPointArrayRange(xs,pad)
if(~exist('pad'))
    pad = 250;
end
xrange = [1e6 -1e6]; yrange = xrange; zrange = xrange;
for i=1:length(xs)
    
    xrange = [min(xrange(1),xs{i}.pos(1)) max(xrange(2),xs{i}.pos(1))];
    yrange = [min(yrange(1),xs{i}.pos(2)) max(yrange(2),xs{i}.pos(2))];
    zrange = [min(zrange(1),xs{i}.pos(3)) max(zrange(2),xs{i}.pos(3))];
end
xrange = [xrange(1)-pad xrange(2)+pad];
yrange = [yrange(1)-pad yrange(2)+pad];
zrange = [zrange(1)-pad zrange(2)+pad];
range = [xrange; yrange; zrange];
end