function tab_c=sm_cost_matching_bp4_new2(pts_2d,trs,ex,ey,xo,yo,f)
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

[pt_3d,d]=sm_recon_general_2({pt1;pt2},trs(1:2),ex(1:2),ey(1:2),xo(1:2),yo(1:2),f(1:2));
pt2d=sm_backproj_general(pt_3d,trs{3},ex(3),ey(3),xo(3),yo(3),f(3));

%find the smallest error in 3;
a=repmat(pt2d,1,n_seeds)';
b=repmat(reshape(pts_2d{3}',2*n_seeds,1),1,n_seeds*n_seeds);
p2dtmp=reshape(a-b,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp=reshape(errs,n_seeds,n_seeds*n_seeds);
[vtmp,ind]=min(tmp);
pt_2d={pt1;pt2;pts_2d{3}(ind,:)};

%do the recon from 3 points;
[pt_3d,d]=sm_recon_general_2(pt_2d,trs(1:3),ex(1:3),ey(1:3),xo(1:3),yo(1:3),f(1:3));

%backproject to 4;
pt2d=sm_backproj_general(pt_3d,trs{4},ex(4),ey(4),xo(4),yo(4),f(4));

%find the smallest error in 4;
a=repmat(pt2d,1,n_seeds)';
b=repmat(reshape(pts_2d{4}',2*n_seeds,1),1,n_seeds*n_seeds);
p2dtmp=reshape(a-b,2,n_seeds*n_seeds*n_seeds);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp=reshape(errs,n_seeds,n_seeds*n_seeds);
[vtmp,ind]=min(tmp);
pt_2d={pt_2d{1};pt_2d{2};pt_2d{3};pts_2d{4}(ind,:)};

%do the recon from 4 points;
[pt_3d,d]=sm_recon_general_2(pt_2d,trs,ex,ey,xo,yo,f);

%backproject to 4 images and make the cost table;
tt=zeros(n_seeds*n_seeds,1);
for k=1:n_ims
    pt2d=sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
    p2dtmp=pt2d-pt_2d{k};
    errs=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
    tt=tt+errs;
end
tab_c=reshape(tt,n_seeds,n_seeds)';
