function [xo,yo]=sm_nextpt(x,y,I,Es,Ed)

%Ed=Eo(y,x)+pi/2;
d=[0,pi/4,pi/2,pi*3/4,pi,pi*5/4,pi*3/2,pi*7/4];
tmp=mod(abs(d-Ed),2*pi);
[v,ind]=min(tmp);
if ind==1
    ys=[y-1,y,y+1];
    xs=[x+1,x+1,x+1];
elseif ind==2
    ys=[y,y+1,y+1];
    xs=[x+1,x+1,x];
elseif ind==3
    ys=[y+1,y+1,y+1];
    xs=[x-1,x,x+1];
elseif ind==4
    ys=[y+1,y+1,y];
    xs=[x,x-1,x-1];
elseif ind==5
    ys=[y-1,y,y+1];
    xs=[x-1,x-1,x-1];
elseif ind==6
    ys=[y-1,y-1,y];
    xs=[x,x-1,x-1];
elseif ind==7
    ys=[y-1,y-1,y-1];
    xs=[x-1,x,x+1];
else
    ys=[y-1,y-1,y];
    xs=[x,x+1,x+1];
end
tmpI=[I(ys(1),xs(1)),I(ys(2),xs(2)),I(ys(3),xs(3))];
tmpEs=[Es(ys(1),xs(1)),Es(ys(2),xs(2)),Es(ys(3),xs(3))];
a1=find(tmpEs==min(tmpEs));
[v,a2]=min(tmpI(a1));
yo=ys(a1(a2));
xo=xs(a1(a2));

