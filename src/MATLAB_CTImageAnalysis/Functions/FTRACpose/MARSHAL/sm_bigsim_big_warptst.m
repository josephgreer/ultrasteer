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
        [trs_all,pts_3d_truth]=sm_readdata_seeds_bigsim_3d(ndir);
        ps=sm_readdata_seeds_bigsim_2d(ndir);
        for k=1:5
            for we=0:8:80
                k1=3*we/2/600;
                k2=4*we/2/600^2;
                k3=5*we/2/600^3;
                k4=6*we/2/600^4;
                for j=1:10
                    [sd,n,k,we,j]
                    tmp=rand(1)*k1*600/3+rand(1)*k2*600^2/4+rand(1)*k3*600^3/5+rand(1)*k4*600^4/6;
                    ee=we/2-tmp;
                    k5=7*ee/600^5;
                    ps2=cell(6,1);
                    pts_2d_all=cell(6,1);
                    for i=1:6
                        ps2{i}=add_error_warp(ps{i},[k1,k2,k3,k4,k5]);
                    end
                    for i=1:6
                        pts_2d_all{i}=add_error(ps2{i},e/0.25);
                    end
                    res=sm_mc_seeds_bigsim_big(ndir,comb,pts_2d_all,trs_all,pts_3d_truth);
                    nfe=strcat('reswarp_e',int2str(e*100),'_',int2str(k),'_',int2str(we*10),'_',int2str(j),'.txt');
                    dlmwrite(strcat(ndir,nfe),res,'\t');
                end
            end
        end
	end
end
etime(clock,t0)