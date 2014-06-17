%sm_bigsim
t0=clock;
s1='Simulation_Datasets_den_size\1_50__50_0__1\';
%e=0;
ndir=s1;
[trs_all,pts_3d_truth]=sm_readdata_seeds_bigsim_3d(ndir);
pts_2d_all=sm_readdata_seeds_bigsim_2d(ndir);
sm_mc_seeds_bigsim(ndir,pts_2d_all,trs_all,pts_3d_truth);
for e=0.25:0.25:2
    e
	s2=strcat('e',int2str(e*100),'_20000\');
	ndir=strcat(s1,s2);
	pts_2d_all=sm_readdata_seeds_bigsim_2d(ndir);
	sm_mc_seeds_bigsim(ndir,pts_2d_all,trs_all,pts_3d_truth);
end
etime(clock,t0)