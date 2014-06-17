function I_new=sm_imdewarp_ims(fim,A2,n_equ)

I=imread(fim);
[x_max,y_max]=size(I);

x=repmat((1:x_max)',y_max,1);
u=x/x_max;
y=reshape(repmat((1:y_max),x_max,1),x_max*y_max,1);
v=y/y_max;
%x0=repmat(p_center(1,1),x_max*y_max,1);
%u0=x0/x_max;
%y0=repmat(p_center(1,2),x_max*y_max,1);
%v0=y0/y_max;

B=zeros(x_max*y_max,(n_equ+1)^2);
for i=0:n_equ
    Bi=nchoosek(n_equ,i)*(u.^i).*((1-u).^(n_equ-i));
%    Bi0=nchoosek(n_equ,i)*(u0.^i).*((1-u0).^(n_equ-i));
    for j=0:n_equ
        Bj=nchoosek(n_equ,j)*(v.^j).*((1-v).^(n_equ-j));
%        Bj0=nchoosek(n_equ,j)*(v0.^j).*((1-v0).^(n_equ-j));
        Bij=Bi.*Bj;
%        Bij0=Bi0.*Bj0;
        tmp=i*(n_equ+1)+j+1;
        B(:,tmp)=Bij;%-Bij0;
    end
end

E=B*A2;
p_new=[x,y];
%dp_new=p_new-[x0,y0];
%p_old=dp_new+E+[x0,y0];
p_old=p_new+E;

I_new=zeros(x_max,y_max);
N=x_max*y_max;
for i=1:N
    if p_old(i,1)>=1 & p_old(i,1)<=x_max & p_old(i,2)>=1 & p_old(i,2)<=y_max
        I_new(p_new(i,1),p_new(i,2))=I(round(p_old(i,1)),round(p_old(i,2)));
    end
end

I_new=mat2gray(I_new);
