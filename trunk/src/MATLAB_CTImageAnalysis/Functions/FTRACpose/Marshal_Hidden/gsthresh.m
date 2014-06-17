function Tm=gsthresh(i,j,w,m)
%calculate grayscale threshold;

[ysize,xsize]=size(m);

i1=i-w;
if i1<1
    i1=1;
end
i2=i+w;
if i2>ysize
    i2=ysize;
end
j1=j-w;
if j1<1
    j1=1;
end
j2=j+w;
if j2>xsize
    j2=xsize;
end

tmp=m(i1:i2,j1:j2);
tmp=double(reshape(tmp,(i2-i1+1)*(j2-j1+1),1));
md=median(tmp);
sd=std(tmp);
Tm=uint8(md+sd);
