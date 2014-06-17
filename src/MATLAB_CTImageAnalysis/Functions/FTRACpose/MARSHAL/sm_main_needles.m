function [pts_3d,res_re,res_ren]=sm_main_needles(f_dir)
%SM_MAIN2 processes one 3-image combo of one data set.

[pts_2d,trs,cams,pts_3d_truth]=sm_readdata_needles(f_dir);
for i=1:6
    ex(i,1)=cams{i,1}(1);
    ey(i,1)=cams{i,1}(2);
    xo(i,1)=cams{i,1}(3);
    yo(i,1)=cams{i,1}(4);
    f(i,1)=cams{i,1}(5);
end

[pts_3d,ra]=sm_recon_general_2(pts_2d,trs,ex,ey,xo,yo,f);
[re_mean,re_std,re_min,re_max,re,rv]=sm_err_recon(pts_3d,pts_3d_truth);
res_re=[re_mean,re_std,re_min,re_max];

regtr=sm_regtrans(pts_3d,pts_3d_truth(rv,:));
pts_3dnew=sm_regmapping(pts_3d,regtr);
[ren_mean,ren_std,ren_min,ren_max,ren]=sm_err_recon(pts_3dnew,pts_3d_truth);
res_ren=[ren_mean,ren_std,ren_min,ren_max];
