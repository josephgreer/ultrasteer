function L=sm_ransac_line(ps)

N_trial=20;
[N_all,tmp]=size(ps);
if N_all<2
    L=[];
    return;
end
N_samp=round(N_all/5);
if N_samp<2
    N_samp=2;
end
rand('state',100);
d=zeros(N_trial,1);
for i=1:N_trial
    while 1>0
        ind=rand(N_samp,1);
        ind=round(ind*(N_all-1))+1;
        tmp=ind-ind(1);
        if ~isempty(find(tmp~=0))
            break;
        end
    end
    ind=sort(ind);
    samp=ps(ind,:);
    ls(i,:)=least_squares_line(samp);
    for j=1:N_all
        d(i)=d(i)+sm_dist_pointtoline(ps(j,1),ps(j,2),ls(i,1),ls(i,2),ls(i,3));
    end
end
[v,id]=min(d);
L=ls(id,:);
