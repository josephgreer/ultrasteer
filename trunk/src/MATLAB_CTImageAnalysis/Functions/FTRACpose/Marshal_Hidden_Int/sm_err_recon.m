function [e_mean,e_std,e_min,e_max,e_all,e_v]=sm_err_recon(pts_recon,pts_true)
%SM_ERR_RECON  analysis of 3D reconstruction error.
%
%E_MEAN = SM_ERR_RECON(PTS_RECON,PTS_TRUE), where 
%PTS_RECON is an N-by-3 matrix of reconstructed 3D points, 
%PTS_TRUE is an N-by-3 matrix of truly existing 3D poins, 
%returns the mean of the errors in 3D reconstruction.
%
%[E_MEAN,E_STD] = SM_ERR_RECON(...) returns the mean and the
%standard deviation of the errors in 3D reconstruction.
%
%[E_MEAN,E_STD,E_MIN] = SM_ERR_RECON(...) returns the mean, 
%the standard deviation and the minimum of the errors in 3D 
%reconstruction.
%
%[E_MEAN,E_STD,E_MIN,E_MAX] = SM_ERR_RECON(...) returns the mean, 
%the standard deviation, the minimum and the maximum of the errors
%in 3D reconstruction.

%Create the cost matix for 3D reconstruction error.
e_c=sm_cost_err_recon(pts_recon,pts_true);

%Find the matching for reconstruction errors.
[e_v, T] = hungarian(e_c);
e_v = e_v';

[n,tmp]=size(e_v);
c=0;
for i=1:n
    c=c+e_c(i,e_v(i));
end

%Get the error values from the cost matrix.
n=length(e_v);
e_c_new=reshape(e_c',n*n,1);
e_all=e_c_new(n*(0:n-1)'+e_v);

%Calculate the statistics.
e_mean=mean(e_all);
e_std=std(e_all);
e_min=min(e_all);
e_max=max(e_all);
