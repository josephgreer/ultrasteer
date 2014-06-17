function [res_ra,res_bpe,res_re,res_ren]=sm_main_seeds(f_dir,pts_2d_all,trs_all,cams_all,pts_3d_truth,ns_im)
%SM_MAIN2 processes one 3-image combo of one data set.

%,pts_3d_truth
% ns_im=[1,2,3];


[pts_2d_all,trs_all1,cams_all1,pts_3d_truth]=read(f_dir);

% for i = 1:6
% 
% %     computed = cams_all{i};
% %     old = cams_all1{i};
% %     focal = cams_all{i}(5)
% %     computed = trs_all{i}
% %     old = trs_all1{i};
% % 
% %     computed - old
% end

pts_2d=pts_2d_all(ns_im);
trs=trs_all(ns_im);
cams=cams_all(ns_im);
for i=1:ns_im
    ex(i,1)=cams{i,1}(1);
    ey(i,1)=cams{i,1}(2);
    xo(i,1)=cams{i,1}(3);
    yo(i,1)=cams{i,1}(4);
    f(i,1)=cams{i,1}(5);
end



[pts_3d,ra]=sm_recon_general_2(pts_2d(1:3),trs,ex,ey,xo,yo,f);
ra_mean=mean(ra);
ra_std=std(ra);
ra_min=min(ra);
ra_max=max(ra);
res_ra=[ra_mean,ra_std,ra_min,ra_max];

bpe_all=[];
bpe_all_m=[];
bpe_all_u=[];
for i=1:3
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


% c=cell(3,1);
% c{1}=sm_cost_matching_bp3_new2(pts_2d(1:3),trs,ex,ey,xo,yo,f);
% c{2}=sm_cost_matching_bp3_new2({pts_2d{2};pts_2d{3};pts_2d{1}},{trs{2};trs{3};trs{1}},[ex(2);ex(3);ex(1)],[ey(2);ey(3);ey(1)],[xo(2);xo(3);xo(1)],[yo(2);yo(3);yo(1)],[f(2);f(3);f(1)]);
% c{3}=sm_cost_matching_bp3_new2({pts_2d{3};pts_2d{1};pts_2d{2}},{trs{3};trs{1};trs{2}},[ex(3);ex(1);ex(2)],[ey(3);ey(1);ey(2)],[xo(3);xo(1);xo(2)],[yo(3);yo(1);yo(2)],[f(3);f(1);f(2)]);
% % [M,cost]=sm_matching_3im22(pts_2d(1:3),trs,c,ex,ey,xo,yo,f);
% M = [(1:100)',(1:100)',(1:100)'];
% [r,matched,unmatched,m_truth]=sm_matchingrate2(M);
% [n_m,tmp]=size(matched);
% [n_u,tmp]=size(unmatched);
% [n,tmp]=size(M);
% M=[matched;unmatched];
% [pts_3d,ra]=sm_recon_general_2({pts_2d{1}(M(:,1),:);pts_2d{2}(M(:,2),:);pts_2d{3}(M(:,3),:)},trs,ex,ey,xo,yo,f);
% ra_mean=mean(ra);
% ra_std=std(ra);
% ra_min=min(ra);
% ra_max=max(ra);
% res_ra=[ra_mean,ra_std,ra_min,ra_max];
% ra_m=ra(1:n_m,:);
% if n_u>0
%     ra_u=ra(n_m+1:n,:);
% else
%     ra_u=0;
% end
% ra_mean_m=mean(ra_m);
% ra_std_m=std(ra_m);
% ra_min_m=min(ra_m);
% ra_max_m=max(ra_m);
% res_ra_m=[ra_mean_m,ra_std_m,ra_min_m,ra_max_m];
% ra_mean_u=mean(ra_u);
% ra_std_u=std(ra_u);
% ra_min_u=min(ra_u);
% ra_max_u=max(ra_u);
% res_ra_u=[ra_mean_u,ra_std_u,ra_min_u,ra_max_u];
% 
% bpe_all=[];
% bpe_all_m=[];
% bpe_all_u=[];
% for i=1:3
%     p2d=sm_backproj_general(pts_3d,trs{i},ex(i),ey(i),xo(i),yo(i),f(i));
%     [bpe_mean,bpe_std,bpe_min,bpe_max,bpe]=sm_err_backproj_general(p2d,pts_2d{i},ex(i),ey(i));
%     bpe_m=bpe(1:n_m,:);
%     if n_u>0
%         bpe_u=bpe(n_m+1:n,:);
%     else
%         bpe_u=0;
%     end
%     bpe_all=[bpe_all;bpe];
%     bpe_all_m=[bpe_all_m;bpe_m];
%     bpe_all_u=[bpe_all_u;bpe_u];
% end
% bpe_mean=mean(bpe);
% bpe_std=std(bpe);
% bpe_min=min(bpe);
% bpe_max=max(bpe);
% res_bpe=[bpe_mean,bpe_std,bpe_min,bpe_max];
% bpe_mean_m=mean(bpe_m);
% bpe_std_m=std(bpe_m);
% bpe_min_m=min(bpe_m);
% bpe_max_m=max(bpe_m);
% res_bpe_m=[bpe_mean_m,bpe_std_m,bpe_min_m,bpe_max_m];
% bpe_mean_u=mean(bpe_u);
% bpe_std_u=std(bpe_u);
% bpe_min_u=min(bpe_u);
% bpe_max_u=max(bpe_u);
% res_bpe_u=[bpe_mean_u,bpe_std_u,bpe_min_u,bpe_max_u];
% 
% [re_mean,re_std,re_min,re_max,re,rv]=sm_err_recon(pts_3d,pts_3d_truth);
% 
% res_re=[re_mean,re_std,re_min,re_max];
% res_re
% re_m=re(1:n_m,:);
% if n_u>0
%     re_u=re(n_m+1:n,:);
% else
%     re_u=0;
% end
% re_mean_m=mean(re_m);
% re_std_m=std(re_m);
% re_min_m=min(re_m);
% re_max_m=max(re_m);
% res_re_m=[re_mean_m,re_std_m,re_min_m,re_max_m];
% re_mean_u=mean(re_u);
% re_std_u=std(re_u);
% re_min_u=min(re_u);
% re_max_u=max(re_u);
% res_re_u=[re_mean_u,re_std_u,re_min_u,re_max_u];
% 
% regtr=sm_regtrans(pts_3d,pts_3d_truth(rv,:));
% pts_3dnew=sm_regmapping(pts_3d,regtr);
% [ren_mean,ren_std,ren_min,ren_max,ren]=sm_err_recon(pts_3dnew,pts_3d_truth);
% res_ren=[ren_mean,ren_std,ren_min,ren_max];
% ren_m=ren(1:n_m,:);
% if n_u>0
%     ren_u=ren(n_m+1:n,:);
% else
%     ren_u=0;
% end
% ren_mean_m=mean(ren_m);
% ren_std_m=std(ren_m);
% ren_min_m=min(ren_m);
% ren_max_m=max(ren_m);
% res_ren_m=[ren_mean_m,ren_std_m,ren_min_m,ren_max_m];
% ren_mean_u=mean(ren_u);
% ren_std_u=std(ren_u);
% ren_min_u=min(ren_u);
% ren_max_u=max(ren_u);
% res_ren_u=[ren_mean_u,ren_std_u,ren_min_u,ren_max_u];
% 
