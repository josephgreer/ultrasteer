function pim=match_back_proj(ps,tr)

ex=0.25;
ey=0.25;
xo=600;
yo=600;
f=-1000;

[m,n]=size(ps);
pctmp=zeros(m+1,n);
ptmp=[ps;ones(1,n)];
for i=1:n
    pctmp(:,i)=tr*ptmp(:,i);
end
pc=pctmp(1:3,:);

pim=zeros(m-1,n);
for i=1:n
    pim(1,i)=pc(1,i)*f/pc(3,i)/ex+xo;
    pim(2,i)=pc(2,i)*f/pc(3,i)/ey+yo;
end

pim=round(pim);
