%sm_bigsim_big_gridtst
t0=clock;
comb=[3,6,15,16,18];
sd=1.8;
s_sd=strcat('Simulation_Datasets_den_size\',int2str(floor(sd)), '_', int2str(100*(sd-floor(sd))), '__');
pv=50;
s_pv=strcat(int2str(floor(pv)), '_', int2str(100*(pv-floor(pv))), '__');
e=0.5;
resall=zeros(1000,11);
for n=1:5
    s_n=strcat(int2str(n),'\');
    ndir=strcat(s_sd,s_pv,s_n);
    rsn=zeros(200,11);
    for k=1:5
        [n,k]
        rsk=zeros(40,11);
        for te=0:0.5:5
            rsfe=zeros(40,1);
            for j=1:8
                nfe=strcat('resrot_e',int2str(e*100),'_',int2str(k),'_',int2str(te*10),'_',int2str(j),'.txt');
                tmp=load(strcat(ndir,nfe));
                rs=tmp(1:5,2);
                rsfe((j-1)*5+1:j*5,:)=rs;
            end
            rsk(:,round(te/0.5+1))=rsfe;
        end
        rsn((k-1)*40+1:k*40,:)=rsk;
    end
    resall((n-1)*200+1:n*200,:)=rsn;
end
res=zeros(11,3);
for i=1:11
    res(i,1)=0.5*(i-1);
    res(i,2)=mean(resall(:,i));
    res(i,3)=std(resall(:,i));
end

etime(clock,t0)
