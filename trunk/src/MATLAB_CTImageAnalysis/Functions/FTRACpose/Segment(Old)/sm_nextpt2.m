function [xo,yo]=sm_nextpt(x,y,xp,yp,I,Es,Ed)

d=[0,pi/4,pi/2,pi*3/4];
tmp=mod(abs(d-Ed),pi);
[v,ind]=min(tmp);
if ind==1
    ys1=[y-1,y,y+1];
    xs1=[x+1,x+1,x+1];
    ys2=[y-1,y,y+1];
    xs2=[x-1,x-1,x-1];
elseif ind==2
    ys1=[y,y+1,y+1];
    xs1=[x+1,x+1,x];
    ys2=[y-1,y-1,y];
    xs2=[x,x-1,x-1];
elseif ind==3
    ys1=[y+1,y+1,y+1];
    xs1=[x-1,x,x+1];
    ys2=[y-1,y-1,y-1];
    xs2=[x-1,x,x+1];
else
    ys1=[y+1,y+1,y];
    xs1=[x,x-1,x-1];
    ys2=[y-1,y-1,y];
    xs2=[x,x+1,x+1];
end
tmpI=[I(ys1(1),xs1(1)),I(ys1(2),xs1(2)),I(ys1(3),xs1(3))];
tmpEs=[Es(ys1(1),xs1(1)),Es(ys1(2),xs1(2)),Es(ys1(3),xs1(3))];
a1=find(tmpI==min(tmpI));
[v,a2]=min(tmpEs(a1));
yo=ys1(a1(a2));
xo=xs1(a1(a2));
if xo~=xp | yo~=yp
    return;
end
tmpI=[I(ys2(1),xs2(1)),I(ys2(2),xs2(2)),I(ys2(3),xs2(3))];
tmpEs=[Es(ys2(1),xs2(1)),Es(ys2(2),xs2(2)),Es(ys2(3),xs2(3))];
a1=find(tmpI==min(tmpI));
[v,a2]=min(tmpEs(a1));
yo=ys2(a1(a2));
xo=xs2(a1(a2));
