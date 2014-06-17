%$Id: find_allpoints_on_line.m,v 1.3 2006/03/23 21:43:22 jain Exp $

%Function to find all points in a dataset that fit on a given 3D line. The
%threshold determines the notion of fitting. The output is the points on
%the 3D line and the points not on it. The decision is done by computing
%the distance of each point from the line (norm of a certain crossproduct).
%Input is 3x2 for line equation, Nx2 for the points and number for the
%threshold. Output is Mx2 for points on line and (N-M)x2 for residual
%points.
function [line_pts, res_pts] =  find_allpoints_on_line(line_eq, all_pts, LINE_ERR_THRESHOLD)

num_pts = length(all_pts);

crossprod_vects = (cross( all_pts' - repmat(line_eq(:,1), 1, num_pts), repmat(line_eq(:,2), 1, num_pts)));
residual_err = sqrt(sum((crossprod_vects.^2), 1))'/norm(line_eq(:,2));
%residual_err = (sqrt(diag(crossprod_vects'*crossprod_vects)))/norm(line_eq(:,2));

indices = find(residual_err <= LINE_ERR_THRESHOLD);
line_pts = all_pts(indices, :);

indices = find(residual_err > LINE_ERR_THRESHOLD);
res_pts = all_pts(indices, :);

%$Log: find_allpoints_on_line.m,v $
%Revision 1.3  2006/03/23 21:43:22  jain
%Changed the way error was being computed (using a matrix).
%
%Revision 1.2  2005/12/14 21:36:47  jain
%Added the input and output formats in the comments.
%
%Revision 1.1  2005/12/12 23:16:22  jain
%Commiting the first complete version of the code for registering the FTRAC fiducial to the template. Code works and has been tested.
%

