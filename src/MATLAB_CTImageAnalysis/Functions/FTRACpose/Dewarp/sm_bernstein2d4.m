function A=sm_bernstein2d4(p_true,p_warp,x_max,y_max,n_equ)
%dewarping image with Bernstein method;
%p* are n*2 arrays, p(1,:) is the cental point;

[n_p,tmp]=size(p_true);

E=p_true-p_warp;
u=p_warp(:,1)/x_max;
v=p_warp(:,2)/y_max;

B=zeros(n_p,(n_equ+1)^2);
for i=0:n_equ
    Bi=nchoosek(n_equ,i)*(u.^i).*((1-u).^(n_equ-i));
    for j=0:n_equ
        Bj=nchoosek(n_equ,j)*(v.^j).*((1-v).^(n_equ-j));
        Bij=Bi.*Bj;
        tmp=i*(n_equ+1)+j+1;
        B(:,tmp)=Bij;
    end
end

A=pinv(B'*B)*B'*E;
