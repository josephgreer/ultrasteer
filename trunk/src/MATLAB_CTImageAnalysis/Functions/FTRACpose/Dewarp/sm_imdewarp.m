function [I_new,ee]=sm_imdewarp(fim,A1,A2,dp_true,dp_warp,p_center,n_equ)

I=imread(fim);
[x_max,y_max]=size(I);

figure;
imshow(I);
axis on;
hold on;

[np,tmp]=size(dp_true);
p_true=dp_true+repmat(p_center,np,1);
p_warp=dp_warp+repmat(p_center,np,1);

for i=1:np
    plot(p_true(i,2),p_true(i,1),'*','color','y');
    plot(p_warp(i,2),p_warp(i,1),'*');
end

x=repmat((1:x_max)',y_max,1);
u=x/x_max;
y=reshape(repmat((1:y_max),x_max,1),x_max*y_max,1);
v=y/y_max;
x0=repmat(p_center(1,1),x_max*y_max,1);
u0=x0/x_max;
y0=repmat(p_center(1,2),x_max*y_max,1);
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

E=B*A2;
p_new=[x,y];
dp_new=p_new-[x0,y0];
p_old=dp_new+E+[x0,y0];

I_new=zeros(x_max,y_max);
N=x_max*y_max;
for i=1:N
    if p_old(i,1)>=1 & p_old(i,1)<=x_max & p_old(i,2)>=1 & p_old(i,2)<=y_max
        I_new(p_new(i,1),p_new(i,2))=I(round(p_old(i,1)),round(p_old(i,2)));
    end
end

figure;
I_new=mat2gray(I_new);
imshow(I_new);
axis on;
hold on;

u=p_warp(:,1)/x_max;
v=p_warp(:,2)/y_max;
x0=repmat(p_center(1,1),np,1);
u0=x0/x_max;
y0=repmat(p_center(1,2),np,1);
v0=y0/y_max;

B=zeros(np,(n_equ+1)^2);
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

E=B*A1;
p_dewarp=dp_warp+E+[x0,y0];
for i=1:np
    plot(p_true(i,2),p_true(i,1),'*','color','y');
    plot(p_dewarp(i,2),p_dewarp(i,1),'*');
end
ee=p_dewarp-p_true;

