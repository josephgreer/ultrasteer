%$Id: Point_Cloud_Registration_Error_Function.m,v 1.1 2006/11/21 18:37:26 jain Exp $

%This is the error function that computes the error between two point
%clouds A and B. The current estimate is the transform that converts a 
%point in frame A to that in frame B. Current estimnate can be passed as a
%6x1 vector (3 euler rotations, 3 translations) or as a 4x4 transform. It
%does an hungarian matching between the two clouds and finds the lowest
%cost match. err_vec is the vector of costs (euc distances). matching is
%the matching(i) gives the correspondence of point i in cloud A to the
%index in cloud B.
function [err_vec, matching] = Point_Cloud_Registration_Error_Function(current_estimate, Points_wrt_A, Points_wrt_B)

if( size(current_estimate,2)==1 )
    xform = xyz_xform(current_estimate(1:3), current_estimate(4:6));
elseif( size(current_estimate,2)==4 )
    xform = current_estimate;
else
    display 'Error'
end

Num_Seeds = size(Points_wrt_A, 1);
xformed_seeds = xform*[Points_wrt_A ones(Num_Seeds, 1)]';
xformed_seeds = xformed_seeds(1:3, :)';

Cost_Matrix = [];
for counter = 1 : Num_Seeds
    this_seed = xformed_seeds(counter, :);
    errs = repmat(this_seed, Num_Seeds, 1) - Points_wrt_B;
    errs = sqrt(sum(errs.^2, 2))';
    Cost_Matrix = [Cost_Matrix; errs];
end
matching = hungarian(Cost_Matrix);

err_vec = [];
for counter = 1 : Num_Seeds
    err_vec = [err_vec; Cost_Matrix(counter, matching(counter))];
end

%$Log: Point_Cloud_Registration_Error_Function.m,v $
%Revision 1.1  2006/11/21 18:37:26  jain
%First version of the set of files which can compare Ca-rm based seed reconstructions to Ground Truth obtained from CT.
%This function currently needs both the seeds and the FTRAC fiducial to be visible in
%the CT frame. Will write and add a new one that just works with 'relative reconstruction
%error' computation - i.e. where a post-operative CT is taken of the patient. The
%starting function currently is Compute_Reconstruction_Error.
%
