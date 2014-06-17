function [r2,e223_gd,e222_gd,e223_bd,e222_bd]=sm_main_bp_call(pts_2d,trs,ex,ey,xo,yo,f,pts_3d_truth)

[m,cc]=sm_matching_2im(pts_2d,trs,ex,ey,xo,yo,f);
ff=[(1:length(m))',m];
%ff'
[r2,matched2,unmatched2,m2_truth]=sm_matchingrate_2im(ff);
[n_m2,tmp]=size(matched2);
[n_u2,tmp]=size(unmatched2);
pt_new=sm_reorg_pix(pts_2d(1:2),ff);
[p2_m,ds]=sm_recon_2im({pt_new{1}(matched2(1:n_m2,1),:);pt_new{2}(matched2(1:n_m2,1),:)},trs(1:2),ex,ey,xo,yo,f);

[a23_gd,b23_gd,c23_gd,d23_gd,e23_gd]=sm_err_recon(p2_m,pts_3d_truth(find(m2_truth==1),:));
p221=sm_backproj(p2_m,trs{1},ex,ey,xo,yo,f);
[a221_gd,b221_gd,c221_gd,d221_gd,e221_gd]=sm_err_backproj(p221,pt_new{1}(find(m2_truth==1),:));
p222=sm_backproj(p2_m,trs{2},ex,ey,xo,yo,f);
[a222_gd,b222_gd,c222_gd,d222_gd,e222_gd]=sm_err_backproj(p222,pt_new{2}(find(m2_truth==1),:));
e223_gd=[a23_gd,b23_gd,c23_gd,d23_gd];
e222_gd=[(a221_gd+a222_gd)/2,(b221_gd+b222_gd)/2,min([c221_gd,c222_gd]),max([d221_gd,d222_gd])];

e223_bd=[];
e222_bd=[];
if n_u2>0
    [p2_u,ds]=sm_recon_2im({pt_new{1}(unmatched2(1:n_u2,1),:);pt_new{2}(unmatched2(1:n_u2,1),:)},trs(1:2),ex,ey,xo,yo,f);
	[a23_bd,b23_bd,c23_bd,d23_bd,e23_bd]=sm_err_recon(p2_u,pts_3d_truth(find(m2_truth==0),:));
	pu221=sm_backproj(p2_u,trs{1},ex,ey,xo,yo,f);
	[a221_bd,b221_bd,c221_bd,d221_bd,e221_bd]=sm_err_backproj(pu221,pt_new{1}(find(m2_truth==0),:));
	pu222=sm_backproj(p2_u,trs{2},ex,ey,xo,yo,f);
	[a222_bd,b222_bd,c222_bd,d222_bd,e222_bd]=sm_err_backproj(pu222,pt_new{2}(find(m2_truth==0),:));
    e223_bd=[a23_bd,b23_bd,c23_bd,d23_bd];
    e222_bd=[(a221_bd+a222_bd)/2,(b221_bd+b222_bd)/2,min([c221_bd,c222_bd]),max([d221_bd,d222_bd])];
end
