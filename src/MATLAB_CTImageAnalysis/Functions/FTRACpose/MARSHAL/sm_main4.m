function [r,ec_gd,e3_gd,e2_gd,ec_bd,e3_bd,e2_bd]=sm_main4(f_dir,i_comb)
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
ns_im1=[2,3,4;...
        2,3,5;...
        2,3,6;...
        2,4,5;...
        2,4,6;...
        2,5,6;...
        3,4,5;...
        3,4,6;...
        3,5,6;...
        1,2,3;...
        1,2,4;...
        1,2,5;...
        1,2,6;...
        1,3,4;...
        1,3,5;...
        1,3,6;...
        1,4,5;...
        1,4,6;...
        1,5,6;...
        4,5,6];
        
pts_2d=pts_2d_all(ns_im(i_comb,:));
%pts_2d0=pts_2d_all(ns_im(i_comb,:));
%err=4;
%pts_2d={add_error(pts_2d0{1},err);add_error(pts_2d0{2},err);add_error(pts_2d0{3},err)};
%pts_2d{1,1}=dlmread('pts2dnew1.txt','\t');
%pts_2d{2,1}=dlmread('pts2dnew2.txt','\t');
%pts_2d{3,1}=dlmread('pts2dnew3.txt','\t');
trs=trs_all(ns_im(i_comb,:));

ex=0.25;
ey=0.25;
xo=600;
yo=600;
f=1000;

c=cell(3,1);
c{1}=sm_cost_matching_bp3_new(pts_2d(1:3),trs,ex,ey,xo,yo,f);
c{2}=sm_cost_matching_bp3_new({pts_2d{2};pts_2d{3};pts_2d{1}},{trs{2};trs{3};trs{1}},ex,ey,xo,yo,f);
c{3}=sm_cost_matching_bp3_new({pts_2d{3};pts_2d{1};pts_2d{2}},{trs{3};trs{1};trs{2}},ex,ey,xo,yo,f);
[fff,cost]=sm_matching_3im2(pts_2d(1:3),trs,c,ex,ey,xo,yo,f);
pt_new=sm_reorg_pix(pts_2d,fff(:,1:3));
[r,matched,unmatched,m_truth]=sm_matchingrate2(fff);
[n_m,tmp]=size(matched);
[n_u,tmp]=size(unmatched);
n_seeds=n_m+n_u;

ra3ideal=zeros(n_seeds,1);
p33=zeros(n_seeds,3);
bpe3ideal=zeros(n_seeds,1);
for i=1:n_seeds
    [p33(i,:),ra3ideal(i)]=sm_recon_core({pts_2d{1}(i,:);pts_2d{2}(i,:);pts_2d{3}(i,:)},trs,ex,ey,xo,yo,f);
    p1=sm_backproj_one(p33(i,:),trs{1},ex,ey,xo,yo,f);
    e1=norm(pts_2d{1}(i,:)-p1)/4;
    p2=sm_backproj_one(p33(i,:),trs{2},ex,ey,xo,yo,f);
    e2=norm(pts_2d{2}(i,:)-p2)/4;
    p3=sm_backproj_one(p33(i,:),trs{3},ex,ey,xo,yo,f);
    e3=norm(pts_2d{3}(i,:)-p3)/4;
    bpe3ideal(i)=(e1+e2+e3)/3;
end
%p3=sm_recon(pts_2d,trs,zeros(n_seeds,1),ex,ey,xo,yo,f);
[a3,b3,c3,d3,eideal_rec3]=sm_err_recon(p33,pts_3d_truth);

init_triplets=[matched;unmatched];
[p3,tmp]=sm_recon({pts_2d{1}(init_triplets(1:n_seeds,1),:);pts_2d{2}(init_triplets(1:n_seeds,2),:);pts_2d{3}(init_triplets(1:n_seeds,3),:)},trs,init_triplets(1:n_seeds,4),ex,ey,xo,yo,f);
[a3,b3,c3,d3,e_rec3]=sm_err_recon(p3,pts_3d_truth);

trip_mat=[ones(n_m,1);zeros(n_u,1)];
ra3=[cost(find(m_truth==1));cost(find(m_truth==0))];
combo_ra2=zeros(n_seeds,2);
ra2=zeros(n_seeds,1);
ra2_mat=zeros(n_seeds,1);
ra_mark=zeros(n_seeds,1);
p_rec2=zeros(n_seeds,3);
for i=1:n_seeds
    [p11,d1]=sm_recon_core({pts_2d{1}(init_triplets(i,1),:);pts_2d{2}(init_triplets(i,2),:)},trs(1:2),ex,ey,xo,yo,f);
    [p22,d2]=sm_recon_core({pts_2d{2}(init_triplets(i,2),:);pts_2d{3}(init_triplets(i,3),:)},trs(2:3),ex,ey,xo,yo,f);
    [p33,d3]=sm_recon_core({pts_2d{3}(init_triplets(i,3),:);pts_2d{1}(init_triplets(i,1),:)},{trs{3};trs{1}},ex,ey,xo,yo,f);
    [ra2(i),ind_ra2]=min([d1;d2;d3]);
    if ind_ra2==1
        a=1;
        b=2;
        p_rec2(i,:)=p11;
    elseif ind_ra2==2
        a=2;
        b=3;
        p_rec2(i,:)=p22;
    else
        a=3;
        b=1;
        p_rec2(i,:)=p33;
    end
    combo_ra2(i,:)=[init_triplets(i,a),init_triplets(i,b)];
    if ra2(i)>ra3(i)
        ra_mark(i)=1;
    end
    if combo_ra2(i,1)==combo_ra2(i,2)
        ra2_mat(i)=1;
    end
end
[a2,b2,c2,d2,era_rec2]=sm_err_recon(p_rec2,pts_3d_truth);

bpe3=zeros(n_seeds,1);
bpe2=zeros(n_seeds,1);
combo_bpe2=zeros(n_seeds,2);
bpe2_mat=zeros(n_seeds,1);
bpe_mark=zeros(n_seeds,1);
p_rec2=zeros(n_seeds,3);
for i=1:n_seeds
    [p,tmp]=sm_recon_core({pts_2d{1}(init_triplets(i,1),:);pts_2d{2}(init_triplets(i,2),:);pts_2d{3}(init_triplets(i,3),:)},trs,ex,ey,xo,yo,f);
    p1=sm_backproj_one(p,trs{1},ex,ey,xo,yo,f);
    e1=norm(pts_2d{1}(init_triplets(i,1),:)-p1)/4;
    p2=sm_backproj_one(p,trs{2},ex,ey,xo,yo,f);
    e2=norm(pts_2d{2}(init_triplets(i,2),:)-p2)/4;
    p3=sm_backproj_one(p,trs{3},ex,ey,xo,yo,f);
    e3=norm(pts_2d{3}(init_triplets(i,3),:)-p3)/4;
    bpe3(i)=(e1+e2+e3)/3;

    [p11,tmp]=sm_recon_core({pts_2d{1}(init_triplets(i,1),:);pts_2d{2}(init_triplets(i,2),:)},trs(1:2),ex,ey,xo,yo,f);
    p1=sm_backproj_one(p11,trs{1},ex,ey,xo,yo,f);
    e1=norm(pts_2d{1}(init_triplets(i,1),:)-p1)/4;
    p2=sm_backproj_one(p11,trs{2},ex,ey,xo,yo,f);
    e2=norm(pts_2d{2}(init_triplets(i,2),:)-p2)/4;
    ee1=(e1+e2)/2;
    
    [p22,tmp]=sm_recon_core({pts_2d{2}(init_triplets(i,2),:);pts_2d{3}(init_triplets(i,3),:)},trs(2:3),ex,ey,xo,yo,f);
    p1=sm_backproj_one(p22,trs{2},ex,ey,xo,yo,f);
    e1=norm(pts_2d{2}(init_triplets(i,2),:)-p1)/4;
    p2=sm_backproj_one(p22,trs{3},ex,ey,xo,yo,f);
    e2=norm(pts_2d{3}(init_triplets(i,3),:)-p2)/4;
    ee2=(e1+e2)/2;
    
    [p33,tmp]=sm_recon_core({pts_2d{3}(init_triplets(i,3),:);pts_2d{1}(init_triplets(i,1),:)},{trs{3};trs{1}},ex,ey,xo,yo,f);
    p1=sm_backproj_one(p33,trs{3},ex,ey,xo,yo,f);
    e1=norm(pts_2d{3}(init_triplets(i,3),:)-p1)/4;
    p2=sm_backproj_one(p33,trs{1},ex,ey,xo,yo,f);
    e2=norm(pts_2d{1}(init_triplets(i,1),:)-p2)/4;
    ee3=(e1+e2)/2;
        
    [bpe2(i),ind_bpe2]=min([ee1;ee2;ee3]);
    if ind_bpe2==1
        a=1;
        b=2;
        p_rec2(i,:)=p11;
    elseif ind_bpe2==2
        a=2;
        b=3;
        p_rec2(i,:)=p22;
    else
        a=3;
        b=1;
        p_rec2(i,:)=p33;
    end
    combo_bpe2(i,:)=[init_triplets(i,a),init_triplets(i,b)];
    if bpe2(i)>bpe3(i)
        bpe_mark(i)=1;
    end
    if combo_bpe2(i,1)==combo_bpe2(i,2)
        bpe2_mat(i)=1;
    end
end
[a2,b2,c2,d2,ebpe_rec2]=sm_err_recon(p_rec2,pts_3d_truth);

erarec_mark=zeros(n_seeds,1);
ebperec_mark=zeros(n_seeds,1);

for i=1:n_seeds
    if era_rec2(i)>e_rec3(i)
        erarec_mark(i)=1;
    end
    if ebpe_rec2(i)>e_rec3(i)
        ebperec_mark(i)=1;
    end
end
M=[init_triplets,trip_mat,ra3,ra2,combo_ra2,ra2_mat,ra_mark,bpe3,bpe2,combo_bpe2,bpe2_mat,bpe_mark,e_rec3,era_rec2,erarec_mark,ebpe_rec2,ebperec_mark]
M2=[mean(eideal_rec3),mean(ra3ideal),mean(bpe3ideal),std(eideal_rec3),std(ra3ideal),std(bpe3ideal);...
        mean(e_rec3),mean(ra3),mean(bpe3),std(e_rec3),std(ra3),std(bpe3);...
        mean(e_rec3(1:n_m)),mean(ra3(1:n_m)),mean(bpe3(1:n_m)),std(e_rec3(1:n_m)),std(ra3(1:n_m)),std(bpe3(1:n_m));...
        mean(e_rec3(n_m+1:n_seeds)),mean(ra3(n_m+1:n_seeds)),mean(bpe3(n_m+1:n_seeds)),std(e_rec3(n_m+1:n_seeds)),std(ra3(n_m+1:n_seeds)),std(bpe3(n_m+1:n_seeds))]

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

