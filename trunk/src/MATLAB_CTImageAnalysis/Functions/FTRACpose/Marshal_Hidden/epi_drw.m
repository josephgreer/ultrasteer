function epi_drw(pL,imL,transL,imR,transR)

%mL=load(imL);
%mR=load(imR);
figure;
subplot(2,2,1);
imshow(imL);
axis on
%plot(mL(:,1),mL(:,2),'s');
hold on;
plot(pL(2),pL(1),'s');
subplot(2,2,2);
imshow(imR);
axis on
%plot(mR(:,1),mR(:,2),'s');
hold on;

ex=0.25;
ey=0.25;
xo=600;
yo=600;
f=1000;

%tL is the transform from world to left
%tR is the transform from world to right
tL=load(transL);
tR=load(transR);
tRtoL=tL*inv(tR);
tLtoR=inv(tRtoL);

pL1=[(pL(1)-xo)*ex;(pL(2)-yo)*ey;-f];

pR1=tLtoR*[pL1;1];
pR0=tLtoR*[0,0,0,1]';
pRm1=round(pR1(1:2,:)/pR1(3)*(-f)./[ex;ey]+[xo;yo]);
pRm0=round(pR0(1:2,:)/pR0(3)*(-f)./[ex;ey]+[xo;yo]);

plot([pRm0(2),pRm1(2)],[pRm0(1),pRm1(1)]);
m=imread(imL);
[a,b]=size(m);
axis([0 a 0 b]);