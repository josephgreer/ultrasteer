%$Id: fit_least_square_3dline.m,v 1.2 2005/12/14 21:36:47 jain Exp $

%Function to fit a 3D least squares line to a set of 3D points. Basically
%the idea is from statistics. PCA gives the projection direction with
%minimum norm - use that as the direction. Use the mean as a point on the
%line. logic: projected mean of data is the mean of projection. For any
%given projection direction, choosing the mean point minimizes the l2
%error. Thus mean point in the point. Given the mean point, PCA provides
%the direction with minimum variance (adapted l2 norm). Thus this is the l2
%fit. The input is an Nx3 matrix of the points and the output is the
%equation of the line (3x2) and the residual error.
function [line, residual_err] =  fit_least_square_3dline(pts)

num_pts = size(pts,1);

%compute the direction using PCA.
mean_point = mean(pts)';
pts_wrt_mean = pts - repmat(mean_point', num_pts, 1);
cov_matrix = pts_wrt_mean'*pts_wrt_mean;
[eig_vec eig_val] = eig(cov_matrix);
direction = eig_vec(:,3);
if(direction(3)<0)
    direction = -direction;
end
direction = direction/norm(direction);
line = [mean_point direction];

crossprod_vects = (cross( pts' - repmat(mean_point, 1, num_pts), repmat(direction, 1, num_pts)))/norm(direction);
residual_err = mean(sqrt(diag(crossprod_vects'*crossprod_vects)));

%$Log: fit_least_square_3dline.m,v $
%Revision 1.2  2005/12/14 21:36:47  jain
%Added the input and output formats in the comments.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%
