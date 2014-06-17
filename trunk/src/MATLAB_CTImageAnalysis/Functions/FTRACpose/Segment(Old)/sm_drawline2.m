function [p1,p2]=sm_drawline2(imax,jmax,a,b,c)

%a*i+b*j+c=0
if a==0
    j1=-c/b;
    j2=-c/b;
    p1=[1,j1];
    p2=[imax,j2];
elseif b==0
    i1=-c/a;
    i2=-c/a;
    p1=[i1,1];
    p2=[i2,jmax];
else
    tmp=0;
    i1=-(b+c)/a;
    i2=-(b*jmax+c)/a;
    if i1>=1 & i1<=imax
        tmp=tmp+1;
        pp(tmp,:)=[i1,1];
    end
    if i2>=1 & i2<=imax
        tmp=tmp+1;
        pp(tmp,:)=[i2,jmax];
    end
    j1=-(a+c)/b;
    j2=-(a*imax+c)/b;
    if j1>=1 & j1<=jmax
        tmp=tmp+1;
        pp(tmp,:)=[1,j1];
    end
    if j2>=1 & j2<=jmax
        tmp=tmp+1;
        pp(tmp,:)=[imax,j2];
    end
    p1=pp(1,:);
    p2=pp(2,:);
end

%line([p1(2),p2(2)],[p1(1),p2(1)]);
