function tab_c=sm_cost_matching_bp3_2(pts_2d,trs,ex,ey,xo,yo,f)
%SM_COST_MATCHING_BP3_2 generates the cost matrix for 2-image matching 
%based on back projection errors of 3 images.
%
%tab_c=sm_cost_matching_bp3_2(pts_2d,trs,ex,ey,xo,yo,f), 
%where pts_2d is a cell containing 3 N*2 matries for 3 images,
%trs is a cell containing 3 4*4 transform matrices for 3 images, 
%ex and ey are the x,y pixel sizes,
%xO and xO are the x,y image centers,
%f is the focal length.
%sm_cost_matching_bp3_2 returns tab_c as a N*N cost matrix 
%for the matching of the first two images.

%here pts_2d 1 and 2 are prior images, pts_2d 3 is the assisting image.
[n_seeds,tmp]=size(pts_2d{1});
[n_ims,tmp]=size(pts_2d);
tab_c=zeros(n_seeds);

for i=1:n_seeds
    for j=1:n_seeds
        pt_2d={pts_2d{1}(i,:);pts_2d{2}(j,:)};
        [pt_3d,d]=sm_recon_core(pt_2d,trs(1:2),ex,ey,xo,yo,f);
%        if i==j
%            i
%            pt_3d
%            d
%        end
        p2d=sm_backproj_one(pt_3d,trs{3},ex,ey,xo,yo,f);
        errs=pts_2d{3}-repmat(p2d,n_seeds,1);
        e_c=sqrt(errs(:,1).^2+errs(:,2).^2);
        [tmp,m]=min(e_c);
        pt_2d={pt_2d{1};pt_2d{2};pts_2d{3}(m,:)};
        [pt_3d,d]=sm_recon_core(pt_2d,trs,ex,ey,xo,yo,f);
        for k=1:3
            p2d=sm_backproj_one(pt_3d,trs{k},ex,ey,xo,yo,f);
            err=norm(p2d-pt_2d{k});
            tab_c(i,j)=tab_c(i,j)+err;
        end
    end
end
