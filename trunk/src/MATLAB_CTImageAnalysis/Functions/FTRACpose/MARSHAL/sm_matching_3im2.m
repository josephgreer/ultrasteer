function [M,cost]=sm_matching_3im2(pt,tr,c,ex,ey,xo,yo,f)
%SM_MATCHING_3IM2 does 3-image matching.
%
%[M,cost]=sm_matching_3im2(pt,tr,c,pg,ex,ey,xo,yo,f),
%where pt is a cell containing 3 N*2 matries for 3 images,
%tr is a cell containing 3 4*4 transform matrices for 3 images, 
%c is a cell constaining 3 N*N cost matrices for matching,
%ex and ey are the x,y pixel sizes,
%xO and xO are the x,y image centers,
%f is the focal length.
%sm_matching_3im2 returns M as a N*4 matching matrix, 
%cost as a N-dimensional vector of matching accuarcy of all the points.

[N,tmp]=size(pt);
[n_seeds,tmp]=size(pt{1});

res=sm_matching_3im_pre(c);
[n_t,tmp]=size(res);

mm=zeros(n_seeds,N+1);
cc=0;

for j=1:n_t
    [tmp,n_c]=size(res{j});
    n_c=n_c-1;
    if n_c<=N
        cc=cc+1;
        mm(cc,:)=[res{j}(1:N),0];
    else
        k=n_c/N;
        nn=reshape(res{j}(1:n_c),N,k)';
        if k<7
            bb=sm_comb_all(nn);
            cost=sm_comb_all_cost(bb,pt,tr,ex,ey,xo,yo,f);
            if k<=5
                mm(cc+1:cc+k,:)=[sm_combset_match(cost,k,bb),zeros(k,1)];
            else
                mm(cc+1:cc+k,:)=[sm_combset_match_bypart(cost,k,5,bb),zeros(k,1)];
            end
        else
            mm(cc+1:cc+k,:)=sm_match_special(nn,c,pt,tr,ex,ey,xo,yo,f);
        end
        cc=cc+k;
    end
end

cost=zeros(n_seeds,1);
M=zeros(n_seeds,N+1);
M(mm(1:n_seeds,1),:)=mm(1:n_seeds,:);
pt_2d=cell(N,1);
for i=1:n_seeds
    for j=1:N
        pt_2d{j}=pt{j}(M(i,j),:);
    end
    [tmp,cost(i)]=sm_recon_one(pt_2d,tr,M(i,4),ex,ey,xo,yo,f);
end
