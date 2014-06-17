function pim=match_back_proj1(ps,tr)

ex=0.25;
ey=0.25;
xo=600;
yo=600;
f=-1000;

ptmp=[ps;1];
pctmp=tr*ptmp;
pc=pctmp(1:3);

pim(1,1)=pc(1)*f/pc(3)/ex+xo;
pim(2,1)=pc(2)*f/pc(3)/ey+yo;

pim=round(pim);
