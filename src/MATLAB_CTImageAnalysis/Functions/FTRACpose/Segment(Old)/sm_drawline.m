function [p1,p2]=sm_drawline(imax,jmax,r,th)

ic=floor((imax+1)/2);
jc=floor((jmax+1)/2);

i0=r*sin(th);
j0=r*cos(th);

if th==0
    p1=[1,r+jc];
    p2=[imax,r+jc];
elseif th==pi
    p1=[1,-r+jc];
    p2=[imax,-r+jc];
elseif th==pi/2
    p1=[r+ic,1];
    p2=[r+ic,jmax];
elseif th==pi*3/2
    p1=[-r+ic,1];
    p2=[-r+ic,jmax];
else
    pp=zeros(2,2);
    tmp=0;
    if r<=1.0e-5
        j1=(1-(i0+ic))*(-tan(th))+(j0+jc);
        j2=(imax-(i0+ic))*(-tan(th))+(j0+jc);
    else
        j1=(1-(i0+ic))*(-i0/j0)+(j0+jc);
        j2=(imax-(i0+ic))*(-i0/j0)+(j0+jc);
    end
    if j1>=1 & j1<=jmax
        tmp=tmp+1;
        pp(tmp,:)=[1,j1];
    end
    if j2>=1 & j2<=jmax
        tmp=tmp+1;
        pp(tmp,:)=[imax,j2];
    end
    if r<=1.0e-5
        i1=(1-(j0+jc))*(-1/tan(th))+(i0+ic);
        i2=(jmax-(j0+jc))*(-1/tan(th))+(i0+ic);
    else
        i1=(1-(j0+jc))*(-j0/i0)+(i0+ic);
        i2=(jmax-(j0+jc))*(-j0/i0)+(i0+ic);
    end
    if i1>=1 & i1<=imax
        tmp=tmp+1;
        pp(tmp,:)=[i1,1];
    end
    if i2>=1 & i2<=imax
        tmp=tmp+1;
        pp(tmp,:)=[i2,jmax];
    end
    p1=pp(1,:);
    p2=pp(2,:);
end

%line([p1(2),p2(2)],[p1(1),p2(1)]);
