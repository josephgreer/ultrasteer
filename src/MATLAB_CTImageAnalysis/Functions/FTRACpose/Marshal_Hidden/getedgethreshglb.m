function Te=getedgethreshglb(m)
t0=clock;
[ysize,xsize]=size(m);
a=[-1,-2,2,1];

k=0;
for i=1:ysize
    for j=1:xsize
        [i,j]        
		%calculate 0 derivative
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
Te=mn+1.5*sd;
etime(clock,t0)