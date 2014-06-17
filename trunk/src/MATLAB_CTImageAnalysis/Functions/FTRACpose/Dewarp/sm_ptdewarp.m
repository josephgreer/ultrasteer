function p_dewarp=sm_ptdewarp(A,p_warp,x_max,y_max,n_equ)

u=p_warp(:,1)/x_max;
v=p_warp(:,2)/y_max;
[np,tmp]=size(p_warp);
%u0=repmat(p_center(1)/x_max,np,1);
%v0=repmat(p_center(2)/y_max,np,1);

B=zeros(np,(n_equ+1)^2);
for i=0:n_equ
    Bi=nchoosek(n_equ,i)*(u.^i).*((1-u).^(n_equ-i));
%    Bi0=nchoosek(n_equ,i)*(u0.^i).*((1-u0).^(n_equ-i));
    for j=0:n_equ
        Bj=nchoosek(n_equ,j)*(v.^j).*((1-v).^(n_equ-j));
%        Bj0=nchoosek(n_equ,j)*(v0.^j)*((1-v0).^(n_equ-j));
        Bij=Bi.*Bj;
%        Bij0=Bi0*Bj0;
        tmp=i*(n_equ+1)+j+1;
        B(:,tmp)=Bij;%-Bij0;
    end
end

E=B*A;
p_dewarp=p_warp+E;
