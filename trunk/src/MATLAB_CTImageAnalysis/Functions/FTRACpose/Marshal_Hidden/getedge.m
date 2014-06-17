function d=getedge(j,rcv,y,x,ysize,xsize,tbl)
%get the edge of a row

%calculate the derivative of all the points
kern=[1,2,0,-2,-1];
tmp=double(rcv);
n=length(rcv);
c=conv(tmp,kern);
c=round(c(3:n+2));

%find the left edge
le=0;
for i=1:j-1
    if abs(c(j-i))>=abs(c(j)) & c(j-i)>getedgethreshlcl(y,x,tbl,ysize,xsize)
        le=j-i;
        break;
    end
end
if le>0
    oo=le;
    for i=1:oo-1
        if c(oo-i)>=c(le)
            le=oo-i;
        else
            break;
        end
    end
end

%find the right edge
re=n+1;
for i=j+1:n
    if abs(c(i))>=abs(c(j)) & c(i)<-getedgethreshlcl(y,x,tbl,ysize,xsize)
        re=i;
        break;
    end
end
if re<n+1
    oo=re;
    for i=oo+1:n
        if c(i)<=c(re)
            re=i;
        else
            break;
        end
    end
end

%calculate the width
d=re-le;
