%sm_bigsim_big_gridtst
t0=clock;
%comb=[3,6,15,16,18];
%comb=1:20;
comb=[12,13,16,19];
res=zeros(9,5);
i=0;
for sd=1.8:0.1:2.0
    s_sd=strcat('Simulation_Datasets_cone_angle\',int2str(floor(sd)), '_', int2str(100*(sd-floor(sd))), '__');
    for pv=45:5:55
        [sd,pv]
        s_pv=strcat(int2str(floor(pv)), '_', int2str(100*(pv-floor(pv))), '__');
        for ca=90:5:90
            s_ca=strcat(int2str(ca),'__');
            rsn=zeros(100,1);
            for n=1:5
                s_n=strcat(int2str(n),'\');
                ndir=strcat(s_sd,s_pv,s_ca,s_n);
                e=0.5;
                rsk=zeros(20,1);
                for k=1:5
                    nfe=strcat('resca_e',int2str(e*100),'_',int2str(k),'.txt');
                    tmp=load(strcat(ndir,nfe));
                    rs=tmp(1:4,2);
                    rsk((k-1)*4+1:k*4,:)=rs;
                end
                rsn((n-1)*20+1:n*20,:)=rsk;
            end
            i=i+1;
            res(i,:)=[sd,pv,ca,mean(rsn),std(rsn)];
        end
    end
end

etime(clock,t0)