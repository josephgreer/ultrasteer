function res=sm_houghaxislength2(center,ps_Ib,imax,jmax)

[N_p,tmp]=size(ps_Ib);
ic=center(1);
jc=center(2);
th=center(4);
dmin=10;
dmax=100;
dr=0.5;
N_a=round((dmax-dmin)/dr)+1;
N_b=N_a;
R=[cos(th),-sin(th);sin(th),cos(th)];
A=zeros(N_a,N_b);
B=zeros(N_a,N_b);

for k=1:N_p
    di=ps_Ib(k,1)-ic;
    dj=ps_Ib(k,2)-jc;
    L=sqrt(di^2+dj^2);
    if L>=dmin & L<=dmax
        xy=inv(R)*[dj;di];
        i=xy(2);
        j=xy(1);
        for q=1:N_b
            b=dmin+dr*(q-1);
            tmp=1-i^2/b^2;
            if tmp>0 
                a=abs(j)/sqrt(tmp);
                if a>=dmin & a<=dmax
                    qa=round((a-dmin)/dr)+1;
                    if qa>N_a
                        qa=N_a;
                    end
                    A(qa,q)=A(qa,q)+1/sqrt(a^2+b^2);
                    B(qa,q)=B(qa,q)+1;
                end
            end
        end
    end
end

%take top 1 line;
N=N_a*N_b;
tmp1=reshape(A',N,1);
tmpb=reshape(B',N,1);
[v,ind]=sort(tmp1);
a=0;
b=0;
res=[];
for i=1:N
    tmp2=ind(N-i+1);
    if tmpb(tmp2)==0
        break;
    end
    if mod(tmp2,N_b)==0
        a=(tmp2/N_b-1)*dr+dmin;
        b=(N_b-1)*dr+dmin;
    else
        a=floor(tmp2/N_b)*dr+dmin;
        b=(mod(tmp2,N_b)-1)*dr+dmin;
    end
    p1=R*[a;0]+[jc;ic];
    p2=R*[-a;0]+[jc;ic];
    p3=R*[0;b]+[jc;ic];
    p4=R*[0;-b]+[jc;ic];
    if p1(1)<1 | p2(1)<1 | p3(1)<1 | p4(1)<1 |...
            p1(1)>jmax | p2(1)>jmax | p3(1)>jmax | p4(1)>jmax |...
            p1(2)<1 | p2(2)<1 | p3(2)<1 | p4(2)<1 |...
            p1(2)>imax | p2(2)>imax | p3(2)>imax | p4(2)>imax
        continue;
    else
        res=[center,a,b,tmp1(tmp2),tmpb(tmp2)];
        break;
    end
end

