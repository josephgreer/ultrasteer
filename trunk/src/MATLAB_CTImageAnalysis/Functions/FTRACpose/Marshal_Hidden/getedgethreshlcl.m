function Te=getedgethreshlcl(y,x,tbl,ysize,xsize)

[m,n]=size(tbl);

ydiv=m-1;
xdiv=n-1;

xstp=(xsize-1)/xdiv;
ystp=(ysize-1)/ydiv;

yy=zeros(1,ydiv+1);
xx=zeros(1,xdiv+1);

yy(1)=1;
xx(1)=1;
yy(ydiv+1)=ysize;
xx(xdiv+1)=xsize;

for iy=2:ydiv
    yy(iy)=round(1+ystp*(iy-1));
end
for ix=2:xdiv
    xx(ix)=round(1+xstp*(ix-1));
end

for iy=1:ydiv+1
    if y<yy(iy)
        break;
    end
end
ylow=yy(iy-1);
yhigh=yy(iy);

for ix=1:xdiv+1
    if x<xx(ix)
        break;
    end
end
xlow=xx(ix-1);
xhigh=xx(ix);

f1=tbl(iy-1,ix-1);
f2=tbl(iy-1,ix);
f3=tbl(iy,ix-1);
f4=tbl(iy,ix);

Te=(f1+(f2-f1)/(xhigh-xlow)*(x-xlow))+((f3+(f4-f3)/(xhigh-xlow)*(x-xlow))-(f1+(f2-f1)/(xhigh-xlow)*(x-xlow)))/(yhigh-ylow)*(y-ylow);