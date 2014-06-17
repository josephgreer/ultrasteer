function [xi,yi]=sm_initfind(x,y,I,Es)

w=5;
tmpI=I(y-w:y+w,x-w:x+w);
tmpEs=Es(y-w:y+w,x-w:x+w);
[y_min,x_min]=find(tmpI==min(min(tmpI)));
n=length(x_min);
xye=zeros(n,3);
for k=1:n
    xye(k,1)=y+y_min(k)-w-1;
    xye(k,2)=x+x_min(k)-w-1;
    xye(k,3)=Es(xye(k,1),xye(k,2));
end
[v,ind]=min(xye(:,3));
xi=xye(ind,2);
yi=xye(ind,1);
