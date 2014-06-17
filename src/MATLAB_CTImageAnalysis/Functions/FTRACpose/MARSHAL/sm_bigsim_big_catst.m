%sm_bigsim_big_gridtst
t0=clock;
%comb=[3,6,15,16,18];
%comb=1:20;
comb=[12,13,16,19];
for sd=1.8:0.1:2.0
    s_sd=strcat('Simulation_Datasets_cone_angle\',int2str(floor(sd)), '_', int2str(100*(sd-floor(sd))), '__');
    for pv=45:5:55
        s_pv=strcat(int2str(floor(pv)), '_', int2str(100*(pv-floor(pv))), '__');
        for ca=90:5:90
            s_ca=strcat(int2str(ca),'__');
            for n=1:5
                s_n=strcat(int2str(n),'\');
                ndir=strcat(s_sd,s_pv,s_ca,s_n);
                [trs_all,pts_3d_truth]=sm_readdata_seeds_bigsim_3d(ndir);
                ps=sm_readdata_seeds_bigsim_2d(ndir);
                e=0.5;
                for k=1:5
                    pts_2d_all=cell(6,1);
                    for i=1:6
                        if ~isempty(ps{i})
                            pts_2d_all{i}=add_error(ps{i},e/0.25);
                        end
                    end
                    [sd,pv,ca,n,k]
                    res=sm_mc_seeds_bigsim_big(ndir,comb,pts_2d_all,trs_all,pts_3d_truth);
                    nfe=strcat('resca_e',int2str(e*100),'_',int2str(k),'.txt');
                    dlmwrite(strcat(ndir,nfe),res,'\t');
                end
            end
        end
    end
end

etime(clock,t0)