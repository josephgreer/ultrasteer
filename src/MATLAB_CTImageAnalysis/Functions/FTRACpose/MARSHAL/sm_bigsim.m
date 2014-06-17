%sm_bigsim
t0=clock;
for k=1:10
    k
    s1=strcat('dataset_tmp\1_50__65_0__',int2str(k),'\');
    ndir=s1;
    %e=0;
    [trs_all,pts_3d_truth]=sm_readdata_seeds_bigsim_3d(ndir);
    pts_2d_all=sm_readdata_seeds_bigsim_2d(ndir);
    sm_mc_seeds_bigsim(ndir,pts_2d_all,trs_all,pts_3d_truth);
    for e=0.25:0.25:5
        for i=1:5
            s2=strcat('e',int2str(e*100),'_',int2str(i),'\');
            ndir=strcat(s1,s2);
            pts_2d_all=sm_readdata_seeds_bigsim_2d(ndir);
            sm_mc_seeds_bigsim(ndir,pts_2d_all,trs_all,pts_3d_truth);
        end
    end
end
etime(clock,t0)