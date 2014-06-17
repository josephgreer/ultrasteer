%sm_bigsim_big_gridtst
t0=clock;
comb=[3,6,15,16,18];
pv=50;
s_pv=strcat(int2str(floor(pv)), '_', int2str(100*(pv-floor(pv))), '__');
e=0.5;
for sd=1.5:0.3:1.8;
	s_sd=strcat('Simulation_Datasets_den_size\',int2str(floor(sd)), '_', int2str(100*(sd-floor(sd))), '__');
	for n=1:5
        s_n=strcat(int2str(n),'\');
        ndir=strcat(s_sd,s_pv,s_n);
        [ts,pts_3d_truth]=sm_readdata_seeds_bigsim_3d(ndir);
        ps=sm_readdata_seeds_bigsim_2d(ndir);
        pts_2d_all=cell(6,1);
        for k=1:5
            for i=1:6
                pts_2d_all{i}=add_error(ps{i},e/0.25);
            end
            for te=0:0.5:5
                for j=1:8
                    [sd,n,k,te,j]
                    trs_all=cell(6,1);
                    for q=1:6
                        ee=(rand(1)-0.5)*2*te;
                        dt=xyz_xform([0,0,0],[ee/sqrt(27);ee/sqrt(27);ee/sqrt(27)*5]);
                        trs_all{q}=dt*ts{q};
                    end
                    res=sm_mc_seeds_bigsim_big(ndir,comb,pts_2d_all,trs_all,pts_3d_truth);
                    nfe=strcat('restrans_e',int2str(e*100),'_',int2str(k),'_',int2str(te*10),'_',int2str(j),'.txt');
                    dlmwrite(strcat(ndir,nfe),res,'\t');
                end
            end
        end
	end
end
etime(clock,t0)