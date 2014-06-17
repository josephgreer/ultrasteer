function d=sm_dist_pointtoellipse(x,y,xc,yc,th,a,b)

thtmp=atan2(y-yc,x-xc)-th;
R=[cos(thtmp),-sin(thtmp);sin(thtmp),cos(thtmp)];
p=R*[a*cos(thtmp);b*sin(thtmp)]+[xc;yc];
d=norm([x;y]-p);
