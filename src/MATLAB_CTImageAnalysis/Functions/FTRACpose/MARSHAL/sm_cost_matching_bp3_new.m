function tab_c=sm_cost_matching_bp3_new(pts_2d,trs,ex,ey,xo,yo,f)
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

pt1=reshape(repmat(pts_2d{1},1,n_seeds)',2,n_seeds*n_seeds)';
pt2=repmat(pts_2d{2},n_seeds,1);
[pt_3d,d]=sm_recon_general({pt1;pt2},trs(1:2),ex,ey,xo,yo,f);
pt2d=sm_backproj_general(pt_3d,trs{3},ex,ey,xo,yo,f);

%find the smallest error in 3;
a=repmat(pt2d,1,n_seeds)';
b=repmat(reshape(pts_2d{3}',2*n_seeds,1),1,n_seeds*n_seeds);
p2dtmp=reshape(a-b,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp=reshape(errs,n_seeds,n_seeds*n_seeds);
[vtmp,ind]=min(tmp);
pt_2d={pt1;pt2;pts_2d{3}(ind,:)};

%do the recon from 3 points;
[pt_3d,d]=sm_recon_general(pt_2d,trs,ex,ey,xo,yo,f);
%backproject to 3 images and make the cost table;
tt=zeros(n_seeds*n_seeds,1);
for k=1:n_ims
    pt2d=sm_backproj_general(pt_3d,trs{k},ex,ey,xo,yo,f);
    p2dtmp=pt2d-pt_2d{k};
    errs=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
    tt=tt+errs;
end
tab_c=reshape(tt,n_seeds,n_seeds)';
