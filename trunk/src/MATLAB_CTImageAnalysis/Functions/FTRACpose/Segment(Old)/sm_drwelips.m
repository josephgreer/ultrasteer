function sm_drwelips(xc,yc,th,a,b)

R=[cos(th),-sin(th);sin(th),cos(th)];

N_th=100;
dth=2*pi/N_th;
i=zeros(N_th+1,1);
j=zeros(N_th+1,1);
for k=1:N_th+1
    theta=dth*(k-1);
    x=a*cos(theta);
    y=b*sin(theta);
    tmp=R*[x;y];
    j(k)=tmp(1)+xc;
    i(k)=tmp(2)+yc;
end
line(j,i);