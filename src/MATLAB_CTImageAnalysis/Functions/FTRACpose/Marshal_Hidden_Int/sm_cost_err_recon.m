function e_c=sm_cost_err_recon(pts_recon,pts_true)
%SM_COST_ERR_RECON  constructs the cost matrix for
%the reconstruction errors.
%
%E_C = SM_COST_ERR_RECON(PTS_RECON,PTS_TRUE), where
%PTS_RECON is an N-by-3 matrix of reconstructed 3D points, 
%PTS_TRUE is an N-by-3 matrix of truly existing 3D poins, 
%returns the cost matrix of the errors in 3D reconstruction, e_c.



[N_seeds,tmp]=size(pts_recon);
N_big=N_seeds^2;
e_c=ones(N_seeds)*N_big;
for i=1:N_seeds
    err=pts_true-repmat(pts_recon(i,:),N_seeds,1);
    e_c(i,:)=sqrt(err(:,1).^2+err(:,2).^2+err(:,3).^2)';
end
