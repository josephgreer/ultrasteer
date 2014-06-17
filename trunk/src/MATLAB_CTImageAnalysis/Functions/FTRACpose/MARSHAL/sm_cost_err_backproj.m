function e_c=sm_cost_err_backproj(pts_bj,pts_true)
%SM_COST_ERR_BACKPROJ  constructs the cost matrix for the backprojection errors.
%
%E_C = SM_COST_ERR_BACKPROJ(PTS_BJ,PTS_TRUE), where 
%PTS_BJ is an N-by-2 matrix of backprojected 2D points, 
%PTS_TRUE is an N-by-2 matrix of truly existing 2D poins, 
%returns the cost matrix of the errors in 2D backprojection.

N_big=10000;

[N_seeds,tmp]=size(pts_bj);
e_c=ones(N_seeds)*N_big;
for i=1:N_seeds
    err=pts_true-repmat(pts_bj(i,:),N_seeds,1);
    e_c(i,:)=sqrt(err(:,1).^2+err(:,2).^2)';
end
