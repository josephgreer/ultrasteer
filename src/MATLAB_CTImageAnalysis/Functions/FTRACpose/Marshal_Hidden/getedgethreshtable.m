function Te=getedgethreshtable(m,ydiv,xdiv)

[ysize,xsize]=size(m);
a=[-1,-2,2,1];

xstp=(xsize-1)/xdiv;
ystp=(ysize-1)/ydiv;

Te=zeros(ydiv+1,xdiv+1);

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
    for ix=1:xdiv+1
        [iy,ix]
		k=0;
        dd=0;
		for i=round(yy(iy)-ystp/2):round(yy(iy)+ystp/2)
            if i<1 | i>ysize
                continue;
            end
            for j=round(xx(ix)-xstp/2):round(xx(ix)+xstp/2)
                if j<1 | j>xsize
                    continue;
                end
				x=zeros(1,4);
				if j-2>=1
                    x(1)=m(i,j-2);
				end
				if j-1>=1
                    x(2)=m(i,j-1);
				end
				if j+1<=xsize
                    x(3)=m(i,j+1);
				end
				if j+2<=xsize
                    x(4)=m(i,j+2);
				end
				d0=sum(a.*x);
				
				%calculate 90 derivative
				x=zeros(1,4);
				if i-2>=1
                    x(1)=m(i-2,j);
				end
				if i-1>=1
                    x(2)=m(i-1,j);
				end
				if i+1<=ysize
                    x(3)=m(i+1,j);
				end
				if i+2<=ysize
                    x(4)=m(i+2,j);
				end
				d90=sum(a.*x);
				
				%calculate 45 derivative
				x=zeros(1,4);
				if i-2>=1 & j-2>=1
                    x(1)=m(i-2,j-2);
				end
				if i-1>=1 & j-1>=1
                    x(2)=m(i-1,j-1);
				end
				if i+1<=ysize & j+1<=xsize
                    x(3)=m(i+1,j+1);
				end
				if i+2<=ysize & j+2<=xsize
                    x(4)=m(i+2,j+2);
				end
				d45=sum(a.*x);
				
				%calculate 135 derivative
				x=zeros(1,4);
				if i-2>=1 & j+2<=xsize
                    x(1)=m(i-2,j+2);
				end
				if i-1>=1 & j+1<=xsize
                    x(2)=m(i-1,j+1);
				end
				if i+1<=ysize & j-1>=1
                    x(3)=m(i+1,j-1);
				end
				if i+2<=ysize & j-2>=1
                    x(4)=m(i+2,j-2);
				end
				d135=sum(a.*x);
                
                k=k+1;
                dd(k,1)=d0;
                dd(k,2)=d45;
                dd(k,3)=d90;
                dd(k,4)=d135;
            end
		end
		tmp=abs(reshape(dd,k*4,1));
		mn=mean(tmp);
		sd=std(tmp);
		Te(iy,ix)=mn+1.5*sd;
    end
end