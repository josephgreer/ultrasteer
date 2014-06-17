function [res_ra,res_re,res_bpe]=sm_main_fid3(f_dir,i_comb)
%SM_MAIN2 processes one 3-image combo of one data set.
%
%[r,ec_gd,e3_gd,e2_gd,ec_bd,e3_bd,e2_bd]=sm_main2(f_dir,i_comb),
%where f_dir is the path of the directory of the data set,
%i_comb is the index of the 3-image combo in process.
%sm_main2 returns r as the rate of successful matching,
%ec_gd as the statistics of the costs of good matches,
%e3_gd as the statistics of the reconstruction errors of good matches,
%e2_gd as the statistics of the back projection errors of good matches,
%ec_bd as the statistics of the costs of bad matches,
%e3_bd as the statistics of the reconstruction errors of bad matches,
%e2_bd as the statistics of the back projection errors of bad matches,
%each statistic quantity includes mean, standard deviation, minimum, and maximum.

[pts_2d_all,trs_all,cams_all,pts_3d_truth]=sm_readdata_fid(f_dir);

ns_im=[1,2,3;...
        1,2,4;...
        1,2,5;...
        1,3,4;...
        1,3,5;...
        1,4,5;...
        2,3,4;...
        2,3,5;...
        2,4,5;...
        3,4,5];
        
pts_2d=pts_2d_all(ns_im(i_comb,:));
trs=trs_all(ns_im(i_comb,:));
cams=cams_all(ns_im(i_comb,:));

[pts_3d,ra]=sm_recon_2im_general(pts_2d,trs,cams);
ra_mean=mean(ra);
ra_std=std(ra);
ra_min=min(ra);
ra_max=max(ra);
res_ra=[ra_mean,ra_std,ra_min,ra_max];

[re_mean,re_std,re_min,re_max,re]=sm_err_recon(pts_3d,pts_3d_truth);
res_re=[re_mean,re_std,re_min,re_max];

ex=cams{1}(1);
ey=cams{1}(2);
xo=cams{1}(3);
yo=cams{1}(4);
f=cams{1}(5);
p1=sm_backproj(pts_3d,trs{1},ex,ey,xo,yo,f);
[bpe1_mean,bpe1_std,bpe1_min,bpe1_max,bpe1]=sm_err_backproj_general(p1,pts_2d{1},ex,ey);
ex=cams{2}(1);
ey=cams{2}(2);
xo=cams{2}(3);
yo=cams{2}(4);
f=cams{2}(5);
p2=sm_backproj(pts_3d,trs{2},ex,ey,xo,yo,f);
[bpe2_mean,bpe2_std,bpe2_min,bpe2_max,bpe2]=sm_err_backproj_general(p2,pts_2d{2},ex,ey);
ex=cams{3}(1);
ey=cams{3}(2);
xo=cams{3}(3);
yo=cams{3}(4);
f=cams{3}(5);
p3=sm_backproj(pts_3d,trs{3},ex,ey,xo,yo,f);
[bpe3_mean,bpe3_std,bpe3_min,bpe3_max,bpe3]=sm_err_backproj_general(p3,pts_2d{3},ex,ey);
bpe=[bpe1;bpe2;bpe3];
bpe_mean=mean(bpe);
bpe_std=std(bpe);
bpe_min=min(bpe);
bpe_max=max(bpe);
res_bpe=[bpe_mean,bpe_std,bpe_min,bpe_max];
