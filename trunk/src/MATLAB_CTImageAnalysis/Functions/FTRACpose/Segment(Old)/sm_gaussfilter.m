function Iout=sm_gaussfilter(Iin)

h = fspecial('gaussian',[1,5],1);
I1=double(Iin);
[m,n]=size(I1);
I2=zeros(m,n);
for i=1:m
    tmp=conv(I1(i,:),h);
    I2(i,:)=tmp(3:n+2);
end
for i=1:n
    tmp=conv(I2(:,i),h');
    I2(:,i)=tmp(3:m+2);
end
Iout=uint8(I2);
