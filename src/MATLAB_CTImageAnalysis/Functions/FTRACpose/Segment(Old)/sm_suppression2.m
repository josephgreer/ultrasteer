function Iout=sm_suppression2(Es,Eo)

[m,n]=size(Es);
mes=max(max(Es));
Iout=zeros(m,n);
d=[0,pi/4,pi/2,pi*3/4];
for i=2:m-1
%    i
    for j=2:n-1
        tmp=abs(d-Eo(i,j));
        [v,ind]=min(tmp);
        if ind==1
            if Es(i,j)>Es(i,j-1) | Es(i,j)>Es(i,j+1)
                Iout(i,j)=0;
            else
                Iout(i,j)=mes-Es(i,j);
            end
        elseif ind==2
            if Es(i,j)>Es(i-1,j-1) | Es(i,j)>Es(i+1,j+1)
                Iout(i,j)=0;
            else
                Iout(i,j)=mes-Es(i,j);
            end
        elseif ind==3
            if Es(i,j)>Es(i-1,j) | Es(i,j)>Es(i+1,j)
                Iout(i,j)=0;
            else
                Iout(i,j)=mes-Es(i,j);
            end
        else
            if Es(i,j)>Es(i-1,j+1) | Es(i,j)>Es(i+1,j-1)
                Iout(i,j)=0;
            else
                Iout(i,j)=mes-Es(i,j);
            end
        end
    end
end
