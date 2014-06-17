%sm_bigsim_sdtst
t0=clock;
%ndir=strcat('dataset_tmp\1_50__65_0__1\');
ndir='Simulation_Datasets_den_size\1_50__50_0__1\';
i=1;
nf=strcat(ndir,'resstat.txt');
x=load(nf);
%t(i)=x(21,1);
%r(i)=x(21,2);
t(i)=x(5,1);
r(i)=x(5,2);
for e=0.25:0.25:2
    s=strcat('e',int2str(e*100),'_20000\');
    s=strcat(ndir,s);
    nf=strcat(s,'resstat.txt');
    x=load(nf);
    i=i+1;
%    t(i)=x(21,1);
%    r(i)=x(21,2);
    t(i)=x(5,1);
    r(i)=x(5,2);
end
ds=0:0.25:2.0;
res=[ds',t',r']

etime(clock,t0)