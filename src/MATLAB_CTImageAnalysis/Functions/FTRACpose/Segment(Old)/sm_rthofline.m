function [r,th]=sm_rthofline(p1,p2)

di=p2(1)-p1(1);
dj=p2(2)-p1(2);

th=atan2(di,dj)-pi/2;
th=mod(th,2*pi);

r=p1(1)*sin(th)+p1(2)*cos(th);

if r<0
    th=mod(th+pi,2*pi);
    r=-r;
end
