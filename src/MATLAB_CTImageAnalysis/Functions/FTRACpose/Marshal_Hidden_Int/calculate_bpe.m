function [errs, pt_3d, pts_2d_bp] = calculate_bpe(pts_2d,matched,trs,ex,ey,xo,yo,focal);
% CALCULATE_BPE - calculates backprojection error

[pt_3d, d, d_array] = sm_recon_general_2({pts_2d{1}(matched(:,1),:);pts_2d{2}(matched(:,2),:);pts_2d{3}(matched(:,3),:)},trs,ex,ey,xo,yo,focal);


% backproject to 3 images
errs = 0;
pts_2d_bp = {};
for k = 1:3 
    pt2d = sm_backproj_general(pt_3d,trs{k},ex(k),ey(k),xo(k),yo(k),focal(k));
    pts_2d_bp{k} = pt2d;
    p2dtmp = pt2d - pts_2d{k}(matched(:,k),:);
    errs = errs + sqrt(p2dtmp(:,1).*p2dtmp(:,1) + p2dtmp(:,2).*p2dtmp(:,2));
end