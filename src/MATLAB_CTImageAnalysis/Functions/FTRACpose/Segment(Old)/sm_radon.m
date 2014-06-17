function angle=sm_radon(Ie)
%find the general trend of straight lines using radon transform;

N_top=10;
dth=1;
N_th=180/dth;
theta=0:dth:180;
R=radon(Ie,theta(1:N_th));
ss=zeros(N_th,1);
for k=1:180
    tmp=flipud(sort(R(:,k)));
    ss(k)=sum(tmp(1:N_top));
end
[v,ind]=max(ss);
angle=(ind-1)/180*pi;
