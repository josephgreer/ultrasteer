function ps=sm_pointsofelipse(jc,ic,th,a,b)

R=[cos(th),-sin(th);sin(th),cos(th)];
N_th=100;
ps=zeros(N_th+1,3);
dth=2*pi/N_th;
for k=1:N_th+1
    theta=dth*(k-1);
    jtmp=a*cos(theta);
    itmp=b*sin(theta);
    tmp=R*[jtmp;itmp];
    j=tmp(1)+jc;
    i=tmp(2)+ic;
    dj=-a*sin(theta);
    di=b*cos(theta);
    orient=atan2(di,dj)+th;
    ps(k,:)=[i,j,orient];
end
