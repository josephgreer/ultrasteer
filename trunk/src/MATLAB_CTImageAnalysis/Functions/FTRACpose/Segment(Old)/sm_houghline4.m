function res=sm_houghline4(E,angle_radon,th_rang,dr,dth,N_line)

[m,n]=size(E);
ic=floor((m+1)/2);
jc=floor((n+1)/2);
a=sqrt((ic-1)^2+(jc-1)^2);
b=sqrt((m-ic)^2+(n-jc)^2);
if a>b
    rmax=round(a);
else
    rmax=round(b);
end
%dr=0.5;
N_r=round(rmax/dr)+1;

%dth=pi/360;
%th_rang=pi/180*5;
thc1=mod(-angle_radon,2*pi);
thc2=mod(pi-angle_radon,2*pi);
th1=mod(thc1-th_rang:dth:thc1+th_rang,2*pi);
th2=mod(thc2-th_rang:dth:thc2+th_rang,2*pi);
%th_all=mod([th1,th2]',2*pi);%change
th_all=sm_cat(th1,th2)';
N_th=length(th_all);
A=zeros(N_r,N_th);

for i=2:m-1
%    i
    for j=2:n-1
       if E(i,j)~=1
           continue;
       end
       for h=1:N_th
           th_d=th_all(h);
           r_d=(i-ic)*sin(th_d)+(j-jc)*cos(th_d);
           if r_d<0
               continue;
           end
           k=round(r_d/dr)+1;
           if k>N_r
               k=N_r;
           end
           A(k,h)=A(k,h)+1;
       end
   end
end

%take top 20 lines;
%N_line=20;
tmp1=reshape(A',N_r*N_th,1);
[v,ind]=sort(tmp1);
for i=1:N_line
    if v(N_r*N_th-i+1)>0
        ii(i,1)=ind(N_r*N_th-i+1);
    else
        break;
    end
end
%tmp2=ind(N_r*N_th-N_line+1:N_r*N_th);
tmp2=flipud(ii);
res0=[floor(tmp2/N_th)*dr,th_all(mod(tmp2,N_th))];
%[v,ind]=sort(res0(:,1));
%res=[v,res0(ind,2)];
tmp3=flipud(v);
res=[res0,tmp3(1:N_line)];
%tow=100;
%p=0;
%for i=1:N_r
%    for j=1:N_th
%        if A(i,j)>tow
%            p=p+1;
%            res(p,:)=[dr*(i-1),dth*(j-1)];
%        end
%    end
%end
