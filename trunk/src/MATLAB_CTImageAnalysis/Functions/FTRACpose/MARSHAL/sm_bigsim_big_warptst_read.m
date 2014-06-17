%sm_bigsim_big_gridtst
t0=clock;
comb=[3,6,15,16,18];
sd=1.8;
s_sd=strcat('Simulation_Datasets_den_size\',int2str(floor(sd)), '_', int2str(100*(sd-floor(sd))), '__');
pv=50;
s_pv=strcat(int2str(floor(pv)), '_', int2str(100*(pv-floor(pv))), '__');
e=0.5;
resall=zeros(1250,11);
for n=1:5
    s_n=strcat(int2str(n),'\');
    ndir=strcat(s_sd,s_pv,s_n);
    rsn=zeros(250,11);
    for k=1:5
        [n,k]
        rsk=zeros(50,11);
        for we=0:8:80
            rsfe=zeros(50,1);
            for j=1:10
                nfe=strcat('reswarp_e',int2str(e*100),'_',int2str(k),'_',int2str(we*10),'_',int2str(j),'.txt');
                tmp=load(strcat(ndir,nfe));
                rs=tmp(1:5,1);
                rsfe((j-1)*5+1:j*5,:)=rs;
            end
            rsk(:,round(we/8+1))=rsfe;
        end
        rsn((k-1)*50+1:k*50,:)=rsk;
    end
    resall((n-1)*250+1:n*250,:)=rsn;
end
res=zeros(11,3);
for i=1:11
    res(i,1)=2*(i-1);
    res(i,2)=mean(resall(:,i));
    res(i,3)=std(resall(:,i));
end

etime(clock,t0)
