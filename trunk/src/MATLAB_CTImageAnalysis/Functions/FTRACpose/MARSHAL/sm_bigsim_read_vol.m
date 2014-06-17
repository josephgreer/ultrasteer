%sm_bigsim_sdtst
t0=clock;
i=0;
for d=35:5:100
    i=i+1;
    ndir=strcat('Simulation_Datasets_den_size\1_50__',int2str(floor(d)), '_', int2str(100*(d-floor(d))), '__1\');
    ndir=strcat(ndir,'e50_30000\');
    nf=strcat(ndir,'resstat.txt');
    x=load(nf);
    t(i)=x(5,1);
    r(i)=x(5,2);
end
ds=35:5:100;
res=[ds',t',r']

etime(clock,t0)