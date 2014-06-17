function [r,e3_gd,e2_gd,e3_bd,e2_bd,r2,e223_gd,e222_gd,e223_bd,e222_bd]=sm_main(f_dir,i_comb)

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

[m,cc]=sm_matching_2im(pts_2d(1:2),trs(1:2),ex,ey,xo,yo,f);
ff=[(1:length(m))',m];
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
e222_gd=[(a221_gd+a222_gd)/2,(b221_gd+b222_gd)/2,(c221_gd+c222_gd)/2,(d221_gd+d222_gd)/2];

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
    e222_bd=[(a221_bd+a222_bd)/2,(b221_bd+b222_bd)/2,(c221_bd+c222_bd)/2,(d221_bd+d222_bd)/2];
end


c=cell(3,1);
c{1}=sm_cost_matching_2im(pts_2d(1:2),trs(1:2),ex,ey,xo,yo,f);
c{2}=sm_cost_matching_2im(pts_2d(2:3),trs(2:3),ex,ey,xo,yo,f);
ppts{1}=pts_2d{3};
ppts{2}=pts_2d{1};
ttrs{1}=trs{3};
ttrs{2}=trs{1};
c{3}=sm_cost_matching_2im(ppts,ttrs,ex,ey,xo,yo,f);

[fff,cost]=sm_matching_3im(pts_2d,trs,c,pts_3d_truth,ex,ey,xo,yo,f);
pt_new=sm_reorg_pix(pts_2d,fff(:,1:3));
[r,matched,unmatched,m_truth]=sm_matchingrate(fff);
[n_m,tmp]=size(matched);
[n_u,tmp]=size(unmatched);

[p_m,ds]=sm_recon({pt_new{1}(matched(1:n_m,1),:);pt_new{2}(matched(1:n_m,1),:);pt_new{3}(matched(1:n_m,1),:)},trs,fff(matched(1:n_m,1),4),ex,ey,xo,yo,f);

[a3_gd,b3_gd,c3_gd,d3_gd,e3_gd]=sm_err_recon(p_m,pts_3d_truth(find(m_truth==1),:));
p21=sm_backproj(p_m,trs{1},ex,ey,xo,yo,f);
[a21_gd,b21_gd,c21_gd,d21_gd,e21_gd]=sm_err_backproj(p21,pt_new{1}(find(m_truth==1),:));
p22=sm_backproj(p_m,trs{2},ex,ey,xo,yo,f);
[a22_gd,b22_gd,c22_gd,d22_gd,e22_gd]=sm_err_backproj(p22,pt_new{2}(find(m_truth==1),:));
p23=sm_backproj(p_m,trs{3},ex,ey,xo,yo,f);
[a23_gd,b23_gd,c23_gd,d23_gd,e23_gd]=sm_err_backproj(p23,pt_new{3}(find(m_truth==1),:));
e3_gd=[a3_gd,b3_gd,c3_gd,d3_gd];
e2_gd=[(a21_gd+a22_gd+a23_gd)/3,(b21_gd+b22_gd+b23_gd)/3,(c21_gd+c22_gd+c23_gd)/3,(d21_gd+d22_gd+d23_gd)/3];

e3_bd=[];
e2_bd=[];
if n_u>0
    [p_u,ds]=sm_recon({pt_new{1}(unmatched(1:n_u,1),:);pt_new{2}(unmatched(1:n_u,1),:);pt_new{3}(unmatched(1:n_u,1),:)},trs,fff(unmatched(1:n_u,1),4),ex,ey,xo,yo,f);
	[a3_bd,b3_bd,c3_bd,d3_bd,e3_bd]=sm_err_recon(p_u,pts_3d_truth(find(m_truth==0),:));
	pu21=sm_backproj(p_u,trs{1},ex,ey,xo,yo,f);
	[a21_bd,b21_bd,c21_bd,d21_bd,e21_bd]=sm_err_backproj(pu21,pt_new{1}(find(m_truth==0),:));
	pu22=sm_backproj(p_u,trs{2},ex,ey,xo,yo,f);
	[a22_bd,b22_bd,c22_bd,d22_bd,e22_bd]=sm_err_backproj(pu22,pt_new{2}(find(m_truth==0),:));
	pu23=sm_backproj(p_u,trs{3},ex,ey,xo,yo,f);
	[a23_bd,b23_bd,c23_bd,d23_bd,e23_bd]=sm_err_backproj(pu23,pt_new{3}(find(m_truth==0),:));
    e3_bd=[a3_bd,b3_bd,c3_bd,d3_bd];
    e2_bd=[(a21_bd+a22_bd+a23_bd)/3,(b21_bd+b22_bd+b23_bd)/3,(c21_bd+c22_bd+c23_bd)/3,(d21_bd+d22_bd+d23_bd)/3];
end

%[p2,ds]=sm_recon(pt_new,trs,fff(:,4),ex,ey,xo,yo,f);
%[a,b,c,d,e]=sm_err_recon(p2,pts_3d_truth);
%p3=sm_backproj(p2,trs{3},ex,ey,xo,yo,f);
%[a2,b2,c2,d2,e2]=sm_err_backproj(p3,pt_new{3});

