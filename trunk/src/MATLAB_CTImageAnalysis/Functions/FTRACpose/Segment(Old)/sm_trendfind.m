function Lt=sm_trendfind(Lc,ang_thresh,trend_thresh)
%find the lines following the general trend;

%1.find the trend;
ths=Lc(:,9);
[N_g,tmp]=size(ths);
cc=zeros(N_g,1);
for i=1:N_g
    tmp=ths-ths(i);
    for j=1:N_g
        dth1=abs(mod(tmp(j),pi));
        dth2=abs(mod(-tmp(j),pi));
        dth=min(dth1,dth2);
        if dth<ang_thresh
            cc(i)=cc(i)+1;
        end
    end
end
[v,ind]=max(cc);
th_trend=Lc(ind,9);

%2.find the lines following the trend;
%th_trendthreshold=pi/180*15;
k=0;
tmp=ths-th_trend;
for j=1:N_g
    dth1=abs(mod(tmp(j),pi));
    dth2=abs(mod(-tmp(j),pi));
    dth=min(dth1,dth2);
    if dth<trend_thresh
        k=k+1;
        Lt(k,:)=Lc(j,:);
    end
end
