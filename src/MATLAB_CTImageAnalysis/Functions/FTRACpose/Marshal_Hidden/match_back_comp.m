function [rr1,rr2]=match_back_comp(ps,tr,pp,n,n_seeds,ff)

rr2=zeros(2,n_seeds);
pim=match_back_proj(ps,tr);
for i=1:n_seeds
    k=i;
    for j=1:n
        k=ff(j,k);
    end
    rr2(:,i)=pim(:,i)-pp(:,k);
end
rr1=pim-pp;
