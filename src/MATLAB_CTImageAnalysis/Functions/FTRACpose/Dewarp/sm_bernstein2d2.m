function A=sm_bernstein2d2(p_true,p_warp,x_max,y_max,n_equ)
%dewarping image with Bernstein method;
%p* are n*2 arrays, p(1,:) is the cental point;

[n_p,tmp]=size(p_true);
dp_true=p_true-repmat(p_true(1,:),n_p,1);
dp_warp=p_warp-repmat(p_warp(1,:),n_p,1);

E=dp_true-dp_warp;
u=p_warp(:,1)/x_max;
v=p_warp(:,2)/y_max;

%A2=Bernstein2D(p_warp',E',5);

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
B=B-repmat(B(1,:),n_p,1);

%C=B'*B;
%[U,S,V]=svd(C);
%r=rank(C);
%[k,tmp]=size(C);
%for i=1:k
%    if i<=r
%        S(i,i)=1/S(i,i);
%    else
%        S(i,i)=0;
%    end
%end
%A=V*S*U'*B'*E;
A=pinv(B'*B)*B'*E;
