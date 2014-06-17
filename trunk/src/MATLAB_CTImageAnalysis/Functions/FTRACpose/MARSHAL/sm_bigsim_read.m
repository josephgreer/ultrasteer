%sm_bigsim_sdtst
t0=clock;
i=0;
for d=1.0:0.1:3.0
%    d
    i=i+1;
    ndir=strcat('Simulation_Datasets_den_size\',int2str(floor(d)), '_', int2str(100*(d-floor(d))), '__50_0__1\');
    ndir=strcat(ndir,'e50_0\');
    nf=strcat(ndir,'resstat.txt');
    x=load(nf);
    t(i)=x(5,1);
    r(i)=x(5,2);
end
ds=1.0:0.1:3.0;
res=[ds',t',r']

etime(clock,t0)