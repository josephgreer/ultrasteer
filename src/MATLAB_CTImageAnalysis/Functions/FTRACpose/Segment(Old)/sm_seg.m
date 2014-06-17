%sm_seg
I0 = imread('IM_1.jpeg');
%figure;
%imshow(I0);
%I1=sm_gaussfilter(I0);
%figure;
%imshow(I1);
[Es,Eo]=sm_enhancer(I0);
I2=mat2gray(Es);
figure;
imshow(I2);


I1=sm_threshfilter(Es,0.6);
figure;
imshow(mat2gray(I1));

tmp=sm_suppression2(Es,Eo);
I3=mat2gray(tmp);
figure;
imshow(I3);

figure;
imshow(I0);
axis on;
hold on;

while(1>0)
[x,y]=ginput(1);
[xi,yi]=sm_initfind(x,y,I0,Es);
plot(xi,yi,'*');

x(1)=xi;
y(2)=yi;
xp=0;
yp=0;
for i=2:20
    Ed=Eo(y(i-1),x(i-1))+pi/2;
    [x(i),y(i)]=sm_nextpt2(x(i-1),y(i-1),xp,yp,I0,Es,Ed);
    plot(x(i),y(i),'*');
    xp=x(i-1);
    yp=y(i-1);
end
[x',y']
end