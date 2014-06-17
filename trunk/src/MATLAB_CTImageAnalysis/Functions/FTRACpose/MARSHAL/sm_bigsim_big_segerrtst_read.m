%sm_bigsim_big_segerrtst
t0=clock;
res=zeros(135,5);
p=0;
for sd=1.5:0.1:1.9
    s_sd=strcat('Simulation_Datasets_den_size\',int2str(floor(sd)), '_', int2str(100*(sd-floor(sd))), '__');
    for pv=45:5:55
        s_pv=strcat(int2str(floor(pv)), '_', int2str(100*(pv-floor(pv))), '__');
        rss0=zeros(40,1);
        rss=zeros(400,8);
        [sd,pv]
        for n=1:8
            s_n=strcat(int2str(n),'\');
            ndir=strcat(s_sd,s_pv,s_n);
            e=0;
            nfe=strcat('res_e',int2str(e*100),'.txt');
            tmp=load(strcat(ndir,nfe));
            rs=tmp(1:5,2);
            rss0((n-1)*5+1:n*5,:)=rs;
            for e=0.25:0.25:2.0
                rsstmp=zeros(50,1);
                for k=1:10
                    nfe=strcat('res_e',int2str(e*100),'_',int2str(k),'.txt');
                    tmp=load(strcat(ndir,nfe));
                    rs=tmp(1:5,2);
                    rsstmp((k-1)*5+1:k*5,:)=rs;
                end
                i=round(e/0.25);
                rss((n-1)*50+1:n*50,i)=rsstmp;
            end
        end
        p=p+1;
        res(p,:)=[sd,pv,0,mean(rss0),std(rss0)];
        for q=1:8
            p=p+1;
            res(p,:)=[sd,pv,0.25*q,mean(rss(:,q)),std(rss(:,q))];
        end
    end
end

etime(clock,t0)