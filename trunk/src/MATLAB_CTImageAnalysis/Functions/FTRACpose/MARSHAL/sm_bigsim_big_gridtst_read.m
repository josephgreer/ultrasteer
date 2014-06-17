%sm_bigsim_big_segerrtst
t0=clock;
res=zeros(170,4);
res_m=zeros(17,10);
res_s=zeros(17,10);
p=0;
aa=0;
for sd=1.0:0.1:2.6
    aa=aa+1;
    s_sd=strcat('Simulation_Datasets_den_size\',int2str(floor(sd)), '_', int2str(100*(sd-floor(sd))), '__');
    bb=0;
    for pv=35:5:80
        bb=bb+1;
        s_pv=strcat(int2str(floor(pv)), '_', int2str(100*(pv-floor(pv))), '__');
        rss0=zeros(125,1);
        [sd,pv]
        for n=1:5
            s_n=strcat(int2str(n),'\');
            ndir=strcat(s_sd,s_pv,s_n);
            e=0.5;
            rsstmp=zeros(25,1);
            for k=1:5
                nfe=strcat('resgrid_e',int2str(e*100),'_',int2str(k),'.txt');
                tmp=load(strcat(ndir,nfe));
                rs=tmp(1:5,2);
                rsstmp((k-1)*5+1:k*5,:)=rs;
            end
            rss0((n-1)*25+1:n*25,1)=rsstmp;
        end
        p=p+1;
        res(p,:)=[sd,pv,mean(rss0),std(rss0)];
        res_m(aa,bb)=mean(rss0);
        res_s(aa,bb)=std(rss0);
    end
end
res_m=[0,35:5:80;(1.0:0.1:2.6)',res_m];
res_s=[0,35:5:80;(1.0:0.1:2.6)',res_s];

etime(clock,t0)