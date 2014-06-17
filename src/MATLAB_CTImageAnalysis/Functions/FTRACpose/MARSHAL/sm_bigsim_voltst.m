%sm_bigsim_sdtst
t0=clock;
e=0.5;
for d=35:5:100
    d
    ndir=strcat('Simulation_Datasets_den_size\1_50__',int2str(floor(d)), '_', int2str(100*(d-floor(d))), '__1\');
    [trs_all,pts_3d_truth]=sm_readdata_seeds_bigsim_3d(ndir);
    ndir=strcat(ndir,'e50_30000\');
    pts_2d_all=sm_readdata_seeds_bigsim_2d(ndir);
    sm_mc_seeds_bigsim(ndir,pts_2d_all,trs_all,pts_3d_truth);
end

etime(clock,t0)