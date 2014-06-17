function [errs, pt_3d, pts_2d_bp] = calculate_bpe_final(pts_2d,matched,trs,ex,ey,xo,yo,focal,N);
% CALCULATE_BPE - calculates backprojection error

n_seeds_all = [];
for i = 1:N
    n_seeds_all(i) = size(pts_2d{i}, 1);
end

pt_2d = {pts_2d{1}(matched(:,1),:);pts_2d{2}(matched(:,2),:);pts_2d{3}(matched(:,3),:)};
[pt_3d, d, d_array] = sm_recon_general_2(pt_2d,trs,ex,ey,xo,yo,focal);

pt2d=sm_backproj_general(pt_3d,trs{3},ex(3),ey(3),xo(3),yo(3),focal(3));
pt2d_length = length(pt2d);

% for more than 3 images
for k = 4:N
    %backproject
    pt2d=sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),focal(k));

    %find the smallest error
    a=repmat(pt2d,1,n_seeds_all(k))';
    b=repmat(reshape(pts_2d{k}',2*n_seeds_all(k),1),1,pt2d_length);
    p2dtmp=reshape(a-b,2,n_seeds_all(k)*pt2d_length);
    errs=sqrt(p2dtmp(1,:).*p2dtmp(1,:)+p2dtmp(2,:).*p2dtmp(2,:));
    tmp=reshape(errs,n_seeds_all(k),pt2d_length);
    [vtmp,ind]=min(tmp, [], 1);
    pt_2d{k} = pts_2d{k}(ind,:);

    %do the recon
    [pt_3d,d] = sm_recon_general_2(pt_2d,trs,ex,ey,xo,yo,focal);
end

% backproject to 3 images
errs = 0;
pts_2d_bp = {};
for k = 1:N
    pt2d = sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),focal(k));
    pts_2d_bp{k} = pt2d;
    p2dtmp = pt2d - pt_2d{k};
    errs = errs + sqrt(p2dtmp(:,1).*p2dtmp(:,1) + p2dtmp(:,2).*p2dtmp(:,2));
end