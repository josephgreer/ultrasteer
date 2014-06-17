function [pts_3d,res_ra,res_bpe,res_re]=sm_main_seeds_4m(f_dir,i_comb)
%SM_MAIN2 processes one 3-image combo of one data set.

[pts_2d_all,trs_all,cams_all,pts_3d_truth]=sm_readdata_seeds(f_dir);
%,pts_3d_truth
ns_im=[1,2,3,4];
        
pts_2d=pts_2d_all(ns_im(i_comb,:));
trs=trs_all(ns_im(i_comb,:));
cams=cams_all(ns_im(i_comb,:));
for i=1:4
    ex(i,1)=cams{i,1}(1);
    ey(i,1)=cams{i,1}(2);
    xo(i,1)=cams{i,1}(3);
    yo(i,1)=cams{i,1}(4);
    f(i,1)=cams{i,1}(5);
end

c=cell(3,1);
c{1}=sm_cost_matching_bp4_new2(pts_2d(1:4),trs,ex,ey,xo,yo,f);
c{2}=sm_cost_matching_bp4_new2({pts_2d{2};pts_2d{3};pts_2d{1};pts_2d{4}},{trs{2};trs{3};trs{1};trs{4}},[ex(2);ex(3);ex(1);ex(4)],[ey(2);ey(3);ey(1);ey(4)],[xo(2);xo(3);xo(1);xo(4)],[yo(2);yo(3);yo(1);yo(4)],[f(2);f(3);f(1);f(4)]);
c{3}=sm_cost_matching_bp4_new2({pts_2d{3};pts_2d{1};pts_2d{2};pts_2d{4}},{trs{3};trs{1};trs{2};trs{4}},[ex(3);ex(1);ex(2);ex(4)],[ey(3);ey(1);ey(2);ey(4)],[xo(3);xo(1);xo(2);xo(4)],[yo(3);yo(1);yo(2);yo(4)],[f(3);f(1);f(2);f(4)]);
[M,cost]=sm_matching_3im22(pts_2d(1:3),trs(1:3),c,ex(1:3),ey(1:3),xo(1:3),yo(1:3),f(1:3));
[pts_3d,ra]=sm_recon_general_2({pts_2d{1}(M(:,1),:);pts_2d{2}(M(:,2),:);pts_2d{3}(M(:,3),:)},trs(1:3),ex(1:3),ey(1:3),xo(1:3),yo(1:3),f(1:3));
ra_mean=mean(ra);
ra_std=std(ra);
ra_min=min(ra);
ra_max=max(ra);
res_ra=[ra_mean,ra_std,ra_min,ra_max];

bpe_all=[];
for i=1:4
    p2d=sm_backproj_general(pts_3d,trs{i},ex(i),ey(i),xo(i),yo(i),f(i));
    [bpe_mean,bpe_std,bpe_min,bpe_max,bpe]=sm_err_backproj_general(p2d,pts_2d{i},ex(i),ey(i));
    bpe_all=[bpe_all;bpe];
end
bpe_mean=mean(bpe);
bpe_std=std(bpe);
bpe_min=min(bpe);
bpe_max=max(bpe);
res_bpe=[bpe_mean,bpe_std,bpe_min,bpe_max];

[re_mean,re_std,re_min,re_max,re]=sm_err_recon(pts_3d,pts_3d_truth);
res_re=[re_mean,re_std,re_min,re_max];
