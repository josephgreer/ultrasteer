%sm_dewarp
t0=clock;
I=imread('dataset_grid\im_00032.jpeg');
[x_max,y_max]=size(I);
%im31
xpx=737;%pixels
ypx=672;
xL=240;
yL=195;
%im32
xpx=781;
ypx=629;
xL=255;
yL=180;
p_true=load('grid31ture.txt');
p_true(:,1)=p_true(:,1)*xpx/xL;
p_true(:,2)=p_true(:,2)*ypx/yL;
p_warp=load('grid32.txt');

figure;
imshow(I);
axis on;
hold on;

[N,tmp]=size(p_warp);
p_true=p_true+repmat(p_warp(1,:),N,1);
for i=1:N
    plot(p_true(i,2),p_true(i,1),'*','color','y');
    plot(p_warp(i,2),p_warp(i,1),'*');
end

n_equ=5;
%A=sm_bernstein2d(p_true,p_warp,x_max,y_max,n_equ);
A2=sm_bernstein2d2(p_warp,p_true,x_max,y_max,n_equ);

%for i=1:N
%    u=p_warp(i,1)/x_max;
%    v=p_warp(i,2)/y_max;
%    B=zeros(1,(n_equ+1)^2);
%    for r=0:n_equ
%        Bi=nchoosek(n_equ,r)*(u^r)*((1-u)^(n_equ-r));
%        for s=0:n_equ
%            Bj=nchoosek(n_equ,s)*(v^s)*((1-v)^(n_equ-s));
%            Bij=Bi*Bj;
%            tmp=r*(n_equ+1)+s+1;
%            B(tmp)=Bij;
%        end
%    end
%    E=B*A;
%    p_new=p_warp(i,:)+E;
%    plot(p_new(2),p_new(1),'*','color','r');
%end
x=repmat((1:x_max)',y_max,1);
u=x/x_max;
y=reshape(repmat((1:y_max),x_max,1),x_max*y_max,1);
v=y/y_max;
x0=repmat(p_warp(1,1),x_max*y_max,1);
u0=x0/x_max;
y0=repmat(p_warp(1,2),x_max*y_max,1);
v0=y0/y_max;

B=zeros(x_max*y_max,(n_equ+1)^2);
for i=0:n_equ
    Bi=nchoosek(n_equ,i)*(u.^i).*((1-u).^(n_equ-i));
    Bi0=nchoosek(n_equ,i)*(u0.^i).*((1-u0).^(n_equ-i));
    for j=0:n_equ
        Bj=nchoosek(n_equ,j)*(v.^j).*((1-v).^(n_equ-j));
        Bj0=nchoosek(n_equ,j)*(v0.^j).*((1-v0).^(n_equ-j));
        Bij=Bi.*Bj;
        Bij0=Bi0.*Bj0;
        tmp=i*(n_equ+1)+j+1;
        B(:,tmp)=Bij-Bij0;
    end
end
%B=B-repmat(B(1,:),x_max*y_max,1);

E=B*A2;
p_new=[x,y];
%[repmat((1:x_max)',y_max,1),reshape(repmat((1:y_max),x_max,1),x_max*y_max,1)];
dp_new=p_new-[x0,y0];
p_old=dp_new+E+[x0,y0];

I_new=zeros(x_max,y_max);
N2=x_max*y_max;
for i=1:N2
    if p_old(i,1)>=1 & p_old(i,1)<=x_max & p_old(i,2)>=1 & p_old(i,2)<=y_max
        I_new(p_new(i,1),p_new(i,2))=I(round(p_old(i,1)),round(p_old(i,2)));
    end
end

figure;
I_new=mat2gray(I_new);
imshow(I_new);
axis on;
hold on;
for i=1:N
    plot(p_true(i,2),p_true(i,1),'*','color','y');
end

etime(clock,t0)