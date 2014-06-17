function [Es,Eo]=sm_enhancer(Iin)

h=[1,0,-1];
I1=double(Iin);
[m,n]=size(I1);
Esx=zeros(m,n);
for i=1:m
    tmp=conv(I1(i,:),h);
    Esx(i,:)=tmp(2:n+1);
end
Esy=zeros(m,n);
for i=1:n
    tmp=conv(I1(:,i),h');
    Esy(:,i)=tmp(2:m+1);
end
Es=sqrt(Esx.^2+Esy.^2);
Eo=mod(atan2(Esy,Esx),pi);