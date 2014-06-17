function res=sm_houghaxislength(center,ps_Ib)

[N_p,tmp]=size(ps_Ib);
ic=center(1);
jc=center(2);
th=center(4);
dmin=10;
dmax=100;
dr=0.5;
N_a=round((dmax-dmin)/dr)+1;
N_b=N_a;
A=zeros(N_a,N_b);

for k=1:N_p
    di=ps_Ib(k,1)-ic;
    dj=ps_Ib(k,2)-jc;
    L=sqrt(di^2+dj^2);
    if L>=dmin & L<=dmax
        R=[cos(th),-sin(th);sin(th),cos(th)];
        xy=inv(R)*[dj;di];
        i=xy(2);
        j=xy(1);
        for q=1:N_b
            b=dmin+dr*(q-1);
            tmp=1-i^2/b^2;
            if tmp>0 
                a=abs(j)/sqrt(tmp);
                if a>=dmin & a<=dmax
                    qa=floor((a-dmin)/dr)+1;
                    if qa>N_a
                        qa=N_a;
                    end
                    A(qa,q)=A(qa,q)+1;
                end
            end
        end
    end
end

%take top 20 lines;
N_line=1;
tmp1=reshape(A',N_a*N_b,1);
[v,ind]=sort(tmp1);
for i=1:N_line
    if v(N_a*N_b-i+1)>=1
        ii(i,1)=ind(N_a*N_b-i+1);
        Ares(i,1)=v(N_a*N_b-i+1);
    else
        break;
    end
end
%tmp2=ind(N_r*N_th-N_line+1:N_r*N_th);
tmp2=ii;
res0=[floor(tmp2/N_b)*dr+dmin,(mod(tmp2,N_b)-1)*dr+dmin];
res=[repmat(center,N_line,1),res0,Ares];
%[v,ind]=sort(res0(:,1));
%res=[v,res0(ind,2),Ares];

