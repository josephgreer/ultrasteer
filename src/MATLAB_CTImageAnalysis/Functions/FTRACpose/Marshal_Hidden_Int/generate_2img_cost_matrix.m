function [tab_c, ind] = generate_2img_cost_matrix(pts_2d,trs,ex,ey,xo,yo,f)
%GENERATE_2IMG_COST_MATRIX generates the cost matrix for 2-image matching 
%based on back projection errors of 3 images.
%
%tab_c=sm_cost_matching_bp3_2(pts_2d,trs,ex,ey,xo,yo,f), 
%where pts_2d is a cell containing 3 N*2 matries for 3 images,
%trs is a cell containing 3 4*4 transform matrices for 3 images, 
%ex and ey are the x,y pixel sizes,
%xO and xO are the x,y image centers,
%f is the focal length.
%GENERATE_2IMG_COST_MATRIX returns tab_c as a N*N cost matrix 
%for the matching of the first two images.
%here pts_2d 1 and 2 are prior images, pts_2d 3 is the assisting image.

n_ims = length(pts_2d);

n_seeds_all = [];
for i = 1:n_ims
    n_seeds_all(i) = size(pts_2d{i}, 1);
end
seed_diff = abs(n_seeds_all(1) - n_seeds_all(2));

% pad the matrix if the seed numbers do not match
if n_seeds_all(1) > n_seeds_all(2)
    for i = 1:seed_diff
        pts_2d{2}(n_seeds_all(2) + i,:) = [0,0];    
    end
    tab_c=zeros(n_seeds_all(1),n_seeds_all(1));
elseif n_seeds_all(1) < n_seeds_all(2)
    for i = 1:seed_diff
        pts_2d{1}(n_seeds_all(1) + i,:) = [0,0];
    end
    tab_c=zeros(n_seeds_all(2),n_seeds_all(2));
else
    n_seeds = size(pts_2d{1}, 1);
    tab_c = zeros(n_seeds,n_seeds);
end

n_seeds = size(pts_2d{1}, 1);

pt1=reshape(repmat(pts_2d{1},1,n_seeds)',2,n_seeds*n_seeds)';
pt2=repmat(pts_2d{2},n_seeds,1);

% reconstruct the 3D points and backprojects them
[pt_3d,d]=sm_recon_general_2({pt1;pt2},trs(1:2),ex(1:2),ey(1:2),xo(1:2),yo(1:2),f(1:2));
pt2d=sm_backproj_general(pt_3d,trs{3},ex(3),ey(3),xo(3),yo(3),f(3));

%find the smallest error in 3;
pt2d_length = length(pt2d);
a=repmat(pt2d,1,n_seeds_all(3))';
b=repmat(reshape(pts_2d{3}',2*n_seeds_all(3),1),1,pt2d_length);
p2dtmp=reshape(a-b,2,n_seeds_all(3)*pt2d_length);
errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
tmp=reshape(errs,n_seeds_all(3),pt2d_length);
[vtmp,ind]=min(tmp, [], 1);
pt_2d={pt1;pt2;pts_2d{3}(ind,:)};

%do the recon from 3 points;
[pt_3d,d]=sm_recon_general_2(pt_2d,trs,ex,ey,xo,yo,f);

% for more than 3 images
for k = 4:n_ims
    %backproject
    pt2d=sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));

    %find the smallest error
    a=repmat(pt2d,1,n_seeds_all(k))';
    b=repmat(reshape(pts_2d{k}',2*n_seeds_all(k),1),1,pt2d_length);
    p2dtmp=reshape(a-b,2,n_seeds_all(k)*pt2d_length);
    errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
    tmp=reshape(errs,n_seeds_all(k),pt2d_length);
    [vtmp,ind]=min(tmp, [], 1);
    pt_2d{k} = pts_2d{k}(ind,:);

    %do the recon
    [pt_3d,d]=sm_recon_general_2(pt_2d,trs,ex,ey,xo,yo,f);
end

% backproject to all images and make the cost table;
tt=zeros(n_seeds*n_seeds,1);
for k=1:n_ims
    pt2d=sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),f(k));
    p2dtmp=pt2d-pt_2d{k};
    errs=sqrt(p2dtmp(:,1).*p2dtmp(:,1)+p2dtmp(:,2).*p2dtmp(:,2));
    tt=tt+errs;
end

% format the cost matrix and remove the costs for padded seeds
tab_c = reshape(tt,n_seeds,n_seeds)';
tab_c = tab_c(1:n_seeds_all(1),1:n_seeds_all(2));    

ind = reshape(ind,n_seeds,n_seeds)';
ind = ind(1:n_seeds_all(1),1:n_seeds_all(2));