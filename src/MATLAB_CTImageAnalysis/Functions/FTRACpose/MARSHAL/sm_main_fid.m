function [r,ec_gd,e3_gd,e2_gd,ec_bd,e3_bd,e2_bd]=sm_main_fid(f_dir,i_comb)
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

%ex=0.25;
%ey=0.25;
%xo=600;
%yo=600;
%f=1000;

c=cell(3,1);
c{1}=sm_cost_matching_bp3_new(pts_2d(1:3),trs,ex,ey,xo,yo,f);
c{2}=sm_cost_matching_bp3_new({pts_2d{2};pts_2d{3};pts_2d{1}},{trs{2};trs{3};trs{1}},ex,ey,xo,yo,f);
c{3}=sm_cost_matching_bp3_new({pts_2d{3};pts_2d{1};pts_2d{2}},{trs{3};trs{1};trs{2}},ex,ey,xo,yo,f);
[fff,cost]=sm_matching_3im2(pts_2d(1:3),trs,c,ex,ey,xo,yo,f);
pt_new=sm_reorg_pix(pts_2d,fff(:,1:3));
[r,matched,unmatched,m_truth]=sm_matchingrate2(fff);

[n_m,tmp]=size(matched);
[n_u,tmp]=size(unmatched);
cost_m=cost(matched(:,1));
ec_gd=[mean(cost_m),std(cost_m),min(cost_m),max(cost_m)];
[p_m,ds]=sm_recon({pt_new{1}(matched(1:n_m,1),:);pt_new{2}(matched(1:n_m,1),:);pt_new{3}(matched(1:n_m,1),:)},trs,fff(matched(1:n_m,1),4),ex,ey,xo,yo,f);

[a3_gd,b3_gd,c3_gd,d3_gd,e3_gd]=sm_err_recon(p_m,pts_3d_truth(find(m_truth==1),:));
e3_gg=e3_gd;
p21=sm_backproj(p_m,trs{1},ex,ey,xo,yo,f);
[a21_gd,b21_gd,c21_gd,d21_gd,e21_gd]=sm_err_backproj(p21,pt_new{1}(find(m_truth==1),:));
p22=sm_backproj(p_m,trs{2},ex,ey,xo,yo,f);
[a22_gd,b22_gd,c22_gd,d22_gd,e22_gd]=sm_err_backproj(p22,pt_new{2}(find(m_truth==1),:));
p23=sm_backproj(p_m,trs{3},ex,ey,xo,yo,f);
[a23_gd,b23_gd,c23_gd,d23_gd,e23_gd]=sm_err_backproj(p23,pt_new{3}(find(m_truth==1),:));
e3_gd=[a3_gd,b3_gd,c3_gd,d3_gd];
e2_gd=[(a21_gd+a22_gd+a23_gd)/3,(b21_gd+b22_gd+b23_gd)/3,min([c21_gd,c22_gd,c23_gd]),max([d21_gd,d22_gd,d23_gd])];
e2_gd=e2_gd*0.25;

ec_bd=[];
e3_bd=[];
e2_bd=[];
if n_u>0
    cost_u=cost(unmatched(:,1));
    ec_bd=[mean(cost_u),std(cost_u),min(cost_u),max(cost_u)];
    [p_u,ds]=sm_recon({pt_new{1}(unmatched(1:n_u,1),:);pt_new{2}(unmatched(1:n_u,1),:);pt_new{3}(unmatched(1:n_u,1),:)},trs,fff(unmatched(1:n_u,1),4),ex,ey,xo,yo,f);
	[a3_bd,b3_bd,c3_bd,d3_bd,e3_bd]=sm_err_recon(p_u,pts_3d_truth(find(m_truth==0),:));
	pu21=sm_backproj(p_u,trs{1},ex,ey,xo,yo,f);
	[a21_bd,b21_bd,c21_bd,d21_bd,e21_bd]=sm_err_backproj(pu21,pt_new{1}(find(m_truth==0),:));
	pu22=sm_backproj(p_u,trs{2},ex,ey,xo,yo,f);
	[a22_bd,b22_bd,c22_bd,d22_bd,e22_bd]=sm_err_backproj(pu22,pt_new{2}(find(m_truth==0),:));
	pu23=sm_backproj(p_u,trs{3},ex,ey,xo,yo,f);
	[a23_bd,b23_bd,c23_bd,d23_bd,e23_bd]=sm_err_backproj(pu23,pt_new{3}(find(m_truth==0),:));
    e3_bd=[a3_bd,b3_bd,c3_bd,d3_bd];
    e2_bd=[(a21_bd+a22_bd+a23_bd)/3,(b21_bd+b22_bd+b23_bd)/3,min([c21_gd,c22_gd,c23_gd]),max([d21_gd,d22_gd,d23_gd])];
    e2_bd=e2_bd*0.25;
end
