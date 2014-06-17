function [e_mean,e_std,e_min,e_max,e_all]=sm_err_backproj(pts_bj,pts_true)
%SM_ERR_backproj  analysis of backprojection error.
%
%E_MEAN = SM_ERR_BACKPROJ(PTS_BJ,PTS_TRUE), where PTS_BJ 
%is an N-by-2 matrix of backprojected 2D points, PTS_TRUE is
%an N-by-2 matrix of truly existing 2D poins, returns the mean
%of the errors in backprojection.
%
%[E_MEAN,E_STD] = SM_ERR_BACKPROJ(...) returns the mean and the
%standard deviation of the errors in backprojection.
%
%[E_MEAN,E_STD,E_MIN] = SM_ERR_BACKPROJ(...) returns the mean, 
%the standard deviation and the minimum of the errors in  
%backprojection.
%
%[E_MEAN,E_STD,E_MIN,E_MAX] = SM_ERR_BACKPROJ(...) returns the mean, 
%the standard deviation, the minimum and the maximum of the errors
%in backprojection.

%Create the cost matix for backprojection error.
e_c=sm_cost_err_backproj(pts_bj,pts_true);

%Find the matching for reconstruction errors.
e_v=sm_hungarian(e_c);

%Get the error values from the cost matrix.
n=length(e_v);
e_c_new=reshape(e_c',n*n,1);
e_all=e_c_new(n*(0:n-1)'+e_v);

%Calculate the statistics.
e_mean=mean(e_all);
e_std=std(e_all);
e_min=min(e_all);
e_max=max(e_all);
