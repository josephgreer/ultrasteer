function [res_ra,res_bpe,res_re,res_ren]=sm_main_seeds(f_dir,pts_2d_all,trs_all,cams_all,pts_3d_truth,ns_im)
%SM_MAIN2 processes one 3-image combo of one data set.

%,pts_3d_truth
% ns_im=[1,2,3];

n_ims = length(ns_im);
pts_2d=pts_2d_all(ns_im);
trs=trs_all(ns_im);
cams=cams_all(ns_im);

for i=1:n_ims
    ex(i,1)=cams{i,1}(1);
    ey(i,1)=cams{i,1}(2);
    xo(i,1)=cams{i,1}(3);
    yo(i,1)=cams{i,1}(4);
    f(i,1)=cams{i,1}(5);
end

[pts_3d,ra]=sm_recon_general_2(pts_2d(1:n_ims),trs,ex,ey,xo,yo,f);
ra_mean=mean(ra);
ra_std=std(ra);
ra_min=min(ra);
ra_max=max(ra);
res_ra=[ra_mean,ra_std,ra_min,ra_max];

bpe_all=[];
bpe_all_m=[];
bpe_all_u=[];
for i=1:n_ims
    p2d=sm_backproj_general(pts_3d,trs{i},ex(i),ey(i),xo(i),yo(i),f(i));
    [bpe_mean,bpe_std,bpe_min,bpe_max,bpe]=sm_err_backproj_general(p2d,pts_2d{i},ex(i),ey(i));
    bpe_all=[bpe_all;bpe];
end
bpe_mean=mean(bpe);
bpe_std=std(bpe);
bpe_min=min(bpe);
bpe_max=max(bpe);
res_bpe=[bpe_mean,bpe_std,bpe_min,bpe_max];

[re_mean,re_std,re_min,re_max,re,rv]=sm_err_recon(pts_3d,pts_3d_truth);
res_re=[re_mean,re_std,re_min,re_max];

regtr=sm_regtrans(pts_3d,pts_3d_truth(rv,:));
pts_3dnew=sm_regmapping(pts_3d,regtr);
[ren_mean,ren_std,ren_min,ren_max,ren]=sm_err_recon(pts_3dnew,pts_3d_truth);
res_ren=[ren_mean,ren_std,ren_min,ren_max];


