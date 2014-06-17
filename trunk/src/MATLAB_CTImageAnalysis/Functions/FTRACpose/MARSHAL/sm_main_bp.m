function [a1,a2,a3,a4]=sm_main_bp(f_dir,i_comb)

[pts_2d_all,trs_all,pts_3d_truth]=sm_readdata(f_dir);

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

ex=0.25;
ey=0.25;
xo=600;
yo=600;
f=1000;
1
[r1,e13_gd,e12_gd,e13_bd,e12_bd]=sm_main_bp_call(pts_2d,trs,ex,ey,xo,yo,f,pts_3d_truth);
a1=[r1,e13_gd,e12_gd,e13_bd,e12_bd];
2
[r2,e23_gd,e22_gd,e23_bd,e22_bd]=sm_main_bp_call({pts_2d{2};pts_2d{3};pts_2d{1}},{trs{2};trs{3};trs{1}},ex,ey,xo,yo,f,pts_3d_truth);
a2=[r2,e23_gd,e22_gd,e23_bd,e22_bd];
3
[r3,e33_gd,e32_gd,e33_bd,e32_bd]=sm_main_bp_call({pts_2d{3};pts_2d{1};pts_2d{2}},{trs{3};trs{1};trs{2}},ex,ey,xo,yo,f,pts_3d_truth);
a3=[r3,e33_gd,e32_gd,e33_bd,e32_bd];
r4=(r1+r2+r3)/3;
e43_gd=sm_avm({e13_gd;e23_gd;e33_gd});
e42_gd=sm_avm({e12_gd;e22_gd;e32_gd});
e43_bd=sm_avm({e13_bd;e23_bd;e33_bd});
e42_bd=sm_avm({e12_bd;e22_bd;e32_bd});
a4=[r4,e43_gd,e42_gd,e43_bd,e42_bd];
%r4=(r1+r2+r3)/3;
%e43_gd=[(e13_gd(1:2)+e23_gd(1:2)+e33_gd(1:2))/3;min([e13_gd(3),e23_gd(3),e33_gd(3)]);max([e13_gd(4),e23_gd(4),e33_gd(4)])];
%e42_gd=[(e12_gd(1:2)+e22_gd(1:2)+e32_gd(1:2))/3;min([e12_gd(3),e22_gd(3),e32_gd(3)]);max([e12_gd(4),e22_gd(4),e32_gd(4)])];
%e43_bd=[(e13_bd(1:2)+e23_bd(1:2)+e33_bd(1:2))/3;min([e13_bd(3),e23_bd(3),e33_bd(3)]);max([e13_bd(4),e23_bd(4),e33_bd(4)])];
%e42_bd=[(e12_bd(1:2)+e22_bd(1:2)+e32_bd(1:2))/3;min([e12_bd(3),e22_bd(3),e32_bd(3)]);max([e12_bd(4),e22_bd(4),e32_bd(4)])];
%[m,cc]=sm_matching_2im(pts_2d,trs,ex,ey,xo,yo,f);
%ff=[(1:length(m))',m];
%ff'
%[r2,matched2,unmatched2,m2_truth]=sm_matchingrate_2im(ff);
%[n_m2,tmp]=size(matched2);
%[n_u2,tmp]=size(unmatched2);
%pt_new=sm_reorg_pix(pts_2d(1:2),ff);
%[p2_m,ds]=sm_recon_2im({pt_new{1}(matched2(1:n_m2,1),:);pt_new{2}(matched2(1:n_m2,1),:)},trs(1:2),ex,ey,xo,yo,f);

%[a23_gd,b23_gd,c23_gd,d23_gd,e23_gd]=sm_err_recon(p2_m,pts_3d_truth(find(m2_truth==1),:));
%p221=sm_backproj(p2_m,trs{1},ex,ey,xo,yo,f);
%[a221_gd,b221_gd,c221_gd,d221_gd,e221_gd]=sm_err_backproj(p221,pt_new{1}(find(m2_truth==1),:));
%p222=sm_backproj(p2_m,trs{2},ex,ey,xo,yo,f);
%[a222_gd,b222_gd,c222_gd,d222_gd,e222_gd]=sm_err_backproj(p222,pt_new{2}(find(m2_truth==1),:));
%e223_gd=[a23_gd,b23_gd,c23_gd,d23_gd];
%e222_gd=[(a221_gd+a222_gd)/2,(b221_gd+b222_gd)/2,(c221_gd+c222_gd)/2,(d221_gd+d222_gd)/2];

%e223_bd=[];
%e222_bd=[];
%if n_u2>0
%    [p2_u,ds]=sm_recon_2im({pt_new{1}(unmatched2(1:n_u2,1),:);pt_new{2}(unmatched2(1:n_u2,1),:)},trs(1:2),ex,ey,xo,yo,f);
%	[a23_bd,b23_bd,c23_bd,d23_bd,e23_bd]=sm_err_recon(p2_u,pts_3d_truth(find(m2_truth==0),:));
%	pu221=sm_backproj(p2_u,trs{1},ex,ey,xo,yo,f);
%	[a221_bd,b221_bd,c221_bd,d221_bd,e221_bd]=sm_err_backproj(pu221,pt_new{1}(find(m2_truth==0),:));
%	pu222=sm_backproj(p2_u,trs{2},ex,ey,xo,yo,f);
%	[a222_bd,b222_bd,c222_bd,d222_bd,e222_bd]=sm_err_backproj(pu222,pt_new{2}(find(m2_truth==0),:));
%    e223_bd=[a23_bd,b23_bd,c23_bd,d23_bd];
%    e222_bd=[(a221_bd+a222_bd)/2,(b221_bd+b222_bd)/2,(c221_bd+c222_bd)/2,(d221_bd+d222_bd)/2];
%end

