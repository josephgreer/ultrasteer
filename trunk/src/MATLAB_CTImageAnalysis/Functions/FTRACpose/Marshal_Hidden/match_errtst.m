function [res0,res00,t_tab,res,res2]=match_errtst(fpt,ftr,ns_im,n_seeds,pg)

t0=clock;
%find all the cost tables;
fpt1=strcat(fpt,int2str(ns_im(1)),'.txt');
fpt2=strcat(fpt,int2str(ns_im(2)),'.txt');
fpt3=strcat(fpt,int2str(ns_im(3)),'.txt');
ftr1=strcat(ftr,int2str(ns_im(1)),'.txt');
ftr2=strcat(ftr,int2str(ns_im(2)),'.txt');
ftr3=strcat(ftr,int2str(ns_im(3)),'.txt');
c=cell(3,1);
c{1}=abs(recon_table(fpt1,ftr1,fpt2,ftr2));
c{2}=abs(recon_table(fpt2,ftr2,fpt3,ftr3));
c{3}=abs(recon_table(fpt3,ftr3,fpt1,ftr1));
pt=cell(3,1);
pt{1}=load(fpt1)';
pt{2}=load(fpt2)';
pt{3}=load(fpt3)';
tr=cell(3,1);
tr{1}=load(ftr1);
tr{2}=load(ftr2);
tr{3}=load(ftr3);
%pg=load(fpg);
t_over=etime(clock,t0);

%3image
%[mmm,t_mat1,t_mat2,c_gd_bf,c_bd_bf,Lp]=match_more_hg52(pt,tr,c,n_seeds,pg);
[fff,t_mat1,t_mat2,c_gd_bf,c_bd_bf,Lp]=match_more_hg52(pt,tr,c,n_seeds,pg);
t1=clock;
%p=match_recon_gen_hg(pt,tr,n_seeds,mmm');
p=match_recon_bat(pt,tr,n_seeds,fff');
t_rec=etime(clock,t1);
t_total=etime(clock,t0);
%[res1,res_3dgd,res_3dbd,res_2dgd_all,res_2dbd_all]=match_comp_3d2d_hg(pt,tr,pg,n_seeds,p,mmm');
[res1,res_3dgd,res_3dbd,res_2dgd_all,res_2dbd_all]=match_comp_3d2d_hg2(pt,tr,pg,n_seeds,p,fff');

%2image
%ppt{1}=pt{1};
%ppt{2}=pt{2};
%ttr{1}=tr{1};
%ttr{2}=tr{2};
%mm3=[fff(:,1),fff(:,2)];
%for i=1:n_seeds
%    mm2(i,:)=mm3(mm3(i,1),:);
%end
%mm2=mm3;
t0=clock;
ppt{1}=pt{1};
ppt{2}=pt{2};
ttr{1}=tr{1};
ttr{2}=tr{2};
f=alg_hungarian2(c{1});
mm2=[1:n_seeds;f];
p2=match_recon_gen_hg_2im(ppt,ttr,n_seeds,mm2);
t_2d=etime(clock,t0);

[res12,res_3dgd2,res_3dbd2,res_2dgd_all2,res_2dbd_all2]=match_comp_3d2d_hg_2im(ppt,ttr,pg,n_seeds,p2,mm2);

res0=[res1,c_gd_bf,c_bd_bf,res12];
res00=[length(Lp),Lp];
t_tab=[t_over,t_mat1,t_mat2,t_rec,t_total,t_2d];
res=[res_3dgd,res_2dgd_all*0.25,res_3dbd,res_2dbd_all*0.25];
res2=[res_3dgd2,res_2dgd_all2*0.25,res_3dbd2,res_2dbd_all2*0.25];
