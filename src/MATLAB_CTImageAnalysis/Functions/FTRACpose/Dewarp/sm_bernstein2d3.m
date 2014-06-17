function A=sm_bernstein2d3(dp_true,dp_warp,p_warp,x_max,y_max,n_equ)
%Dewarping image with Bernstein method
%
%dp_true is an N*2 arrays for "true" relative positions of all dots relative
%to the image center.
%dp_warp is an N*2 arrays for "warped" relative positions of all dots
%relative to the image center.
%p_warp is an N*2 arrays for "warped" absolute positions of all dots in the
%images.
%A is an N*2 arrays of coefficients of Berstein warping function.

[n_p,tmp]=size(dp_true);

E=dp_true-dp_warp;
%p_warp=dp_warp+repmat(p_center,n_p,1);
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
B=B-repmat(B(1,:),n_p,1);

A=pinv(B'*B)*B'*E;
