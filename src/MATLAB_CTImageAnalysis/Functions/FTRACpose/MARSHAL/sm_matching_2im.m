function [MM,C]=sm_matching_2im(pts_2d,trs,ex,ey,xo,yo,f)
%SM_MATCHING_2IM implements seed matching from 2 images.
%
%[M,C]=SM_MATCHING_2IM(PTS,TRS,EX,EY,XO,YO,F), where
%PTS_2D is a cell of 2 N-by-2 matries of 2d points,
%TRS is a cell of 2 4-by-4 transform matrices, 
%EX and EY are the x,y pixel sizes,
%XO and YO are the x,y image centers,
%F is the focal length,
%returns a N-by-1 vector of matching M, 
%and a N-by-1 vector of cost values C.

%construct the cost matrix
%cost=sm_cost_matching_2im(pts_2d,trs,ex,ey,xo,yo,f);
cost=sm_cost_matching_bp3_new2_2im(pts_2d,trs,ex,ey,xo,yo,f);
%cost=sm_cost_matching_bp3_2(pts_2d,trs,ex,ey,xo,yo,f);

%match with hungarian method
M=sm_hungarian(cost);
%get C
[n_seeds,tmp]=size(cost);
v_c=reshape(cost',n_seeds*n_seeds,1);
C=v_c(n_seeds*(0:n_seeds-1)'+M);
MM=[(1:length(M))',M];
