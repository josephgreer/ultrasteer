function dist = angleDist(ang1,ang2)
dist = abs(ang1-ang2);
dist = min(dist, 2*pi-dist);
end