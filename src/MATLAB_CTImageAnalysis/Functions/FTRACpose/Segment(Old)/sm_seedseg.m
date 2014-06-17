function s=sm_seedseg(fim,N)
%segment seeds by hand;

I=imread(fim);
figure;
imshow(I);
axis on;
hold on;

s=zeros(N,2);
for i=1:N
    [x,y]=ginput(1);
    x=round(x);
    y=round(y);
    tmp=I(y-1:y+1,x-1:x+1);
    [v1,m1]=min(tmp);
    [v2,m2]=min(v1);
    s(i,:)=[m1(m2),m2];
    s(i,:)=s(i,:)+[y-1,x-1]-1;
    i
    plot(s(i,2),s(i,1),'*');
end
