%sm_dewarp
t0=clock;
I=imread('dataset_grid\im_00031.jpeg');
[x_max,y_max]=size(I);
xpx=737;%pixels
ypx=672;
xL=240;
yL=195;
p_true=load('grid31ture.txt');
p_true(:,1)=p_true(:,1)*xpx/xL;
p_true(:,2)=p_true(:,2)*ypx/yL;
p_warp=load('grid31.txt');

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
A2=sm_bernstein2d(p_warp,p_true,x_max,y_max,n_equ);

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

u=repmat((1:x_max)'/x_max,y_max,1);
v=reshape(repmat((1:y_max)/y_max,x_max,1),x_max*y_max,1);

B=zeros(x_max*y_max,(n_equ+1)^2);
for i=0:n_equ
    Bi=nchoosek(n_equ,i)*(u.^i).*((1-u).^(n_equ-i));
    for j=0:n_equ
        Bj=nchoosek(n_equ,j)*(v.^j).*((1-v).^(n_equ-j));
        Bij=Bi.*Bj;
        tmp=i*(n_equ+1)+j+1;
        B(:,tmp)=Bij;
    end
end

E=B*A2;
p_new=[repmat((1:x_max)',y_max,1),reshape(repmat((1:y_max),x_max,1),x_max*y_max,1)];
p_old=p_new+E;
%p_old=[repmat((1:x_max)',y_max,1),reshape(repmat((1:y_max),x_max,1),x_max*y_max,1)];
%p_new=p_old+E;

%u=p_new(:,1)/x_max;
%v=p_new(:,2)/y_max;

%B=B*0;%zeros(x_max*y_max,(n_equ+1)^2);
%for i=0:n_equ
%    Bi=nchoosek(n_equ,i)*(u.^i).*((1-u).^(n_equ-i));
%    for j=0:n_equ
%        Bj=nchoosek(n_equ,j)*(v.^j).*((1-v).^(n_equ-j));
%        Bij=Bi.*Bj;
%        tmp=i*(n_equ+1)+j+1;
%        B(:,tmp)=Bij;
%    end
%end

%E=B*A2;
%p_old=p_new+E;

I_new=zeros(x_max,y_max);
N2=x_max*y_max;
for i=1:N2
    if p_old(i,1)>=1 & p_old(i,1)<=x_max & p_old(i,2)>=1 & p_old(i,2)<=y_max
        I_new(p_new(i,1),p_new(i,2))=I(round(p_old(i,1)),round(p_old(i,2)));
    end
end

%I_new=zeros(x_max,y_max);
%N=x_max*y_max;
%for i=1:N
%    if mod(i,100000)==0
%        i
%    end
%    if p_new(i,1)>=1 & p_new(i,1)<=x_max & p_new(i,2)>=1 & p_new(i,2)<=y_max...
%            & p_old(i,1)>=1 & p_old(i,1)<=x_max & p_old(i,2)>=1 & p_old(i,2)<=y_max
%        I_new(round(p_new(i,1)),round(p_new(i,2)))=I(round(p_old(i,1)),round(p_old(i,2)));
%    end
%end

%for i=1:x_max
%    if mod(i,100)==0
%        i
%    end
%    i_min=max(i-1,1);
%    i_max=min(i+1,x_max);
%    for j=1:y_max
%        if I_new(i,j)==0
%            j_min=max(j-1,1);
%            j_max=min(j+1,y_max);
%            I_new(i,j)=mean(mean(I_new(i_min:i_max,j_min:j_max)));
%        end
%    end
%end
%B=zeros(1,(n_equ+1)^2);
%for x=1:x_max
%    if mod(x,100)==0
%        x
%    end
%    for y=1:y_max
%        if I_new(x,y)==0
%            u=x/x_max;
%            v=y/y_max;
%            B=B*0;
%            for i=0:n_equ
%                Bi=nchoosek(n_equ,i)*(u^i)*((1-u)^(n_equ-i));
%                for j=0:n_equ
%                    Bj=nchoosek(n_equ,j)*(v^j)*((1-v)^(n_equ-j));
%                    Bij=Bi*Bj;
%                    tmp=i*(n_equ+1)+j+1;
%                    B(tmp)=Bij;
%                end
%            end
%            E=B*A2;
%            p_old=[x,y]+E;
%            if p_old(1)>=1 & p_old(1)<=x_max & p_old(2)>=1 & p_old(2)<=y_max
%                I_new(x,y)=I(round(p_old(1)),round(p_old(2)));
%end
%        end
%    end
%end

figure;
I_new=mat2gray(I_new);
imshow(I_new);
axis on;
hold on;
for i=1:N
    plot(p_true(i,2),p_true(i,1),'*','color','y');
end

etime(clock,t0)