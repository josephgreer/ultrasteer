function res=sm_houghsymaxis2(Ib)

dmin=10;
dmax=200;

[m,n]=size(Ib);
ic=floor((m+1)/2);
jc=floor((n+1)/2);
a=sqrt((ic-1)^2+(jc-1)^2);
b=sqrt((m-ic)^2+(n-jc)^2);
if a>b
    rmax=round(a);
else
    rmax=round(b);
end
dr=5;%0.5;
N_r=round(rmax/dr)+1;

dth=pi/180*5;%0.5;
N_th=2*pi/dth;

A=zeros(N_r,N_th);
%B=zeros(N_r,N_th);

ps=sm_getallcontourpoints(Ib);

[N_p,tmp]=size(ps);

for i=1:N_p
%    i
    for j=i+1:N_p
        dx=ps(j,2)-ps(i,2);
        dy=ps(j,1)-ps(i,1);
        d=sqrt(dx^2+dy^2);
        if d>=dmin & d<=dmax
            th=atan2(dy,dx);
            x=(ps(i,2)+ps(j,2))/2-jc;
            y=(ps(i,1)+ps(j,1))/2-ic;
%            if abs((ps(i,1)+ps(j,1))/2-58)<2 & (abs(th-pi/2)<pi*2/180 | abs(th-pi*1.5)<pi*2/180)
%                y;
%            end
            r=x*cos(th)+y*sin(th);
            if r<0
                th=atan2(-dy,-dx);
            end
            th=mod(th,2*pi);
            r=abs(r);
            ind_r=round(r/dr)+1;
            if ind_r>N_r
                ind_r=N_r;
            end
            ind_th=round(th/dth)+1;
            if ind_th>N_th
                ind_th=1;
            end
            A(ind_r,ind_th)=A(ind_r,ind_th)+1/d;
%            B(ind_r,ind_th)=B(ind_r,ind_th)+1;
        end
    end
end
%take top 20 lines;
N_line=50;
tmp1=reshape(A',N_r*N_th,1);
[v1,ind]=sort(tmp1);
for i=1:N_line
    if v1(N_r*N_th-i+1)>=1
        ii(i,1)=ind(N_r*N_th-i+1);
    else
        break;
    end
end
%tmp2=ind(N_r*N_th-N_line+1:N_r*N_th);
tmp2=flipud(ii);
if mod(tmp2,N_th)==0
    r=(tmp2/N_th-1)*dr;
    th=mod((N_th-1)*dth,2*pi);
else
    r=floor(tmp2/N_th)*dr;
    th=mod((mod(tmp2,N_th)-1)*dth,2*pi);
end
res0=[r,th];%[floor(tmp2/N_th)*dr,mod((mod(tmp2,N_th)-1)*dth,2*pi)];
Ni=length(ii);
res=[res0,v1(N_r*N_th-Ni+1:N_r*N_th)];

%tmp3=reshape(B',N_r*N_th,1);
%[v2,ind2]=sort(tmp3);
%res=[res,v2(N_r*N_th-Ni+1:N_r*N_th)];