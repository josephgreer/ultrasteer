%$Id: FTRAC_Orientation_Error_Function.m,v 1.1 2006/11/21 18:37:26 jain Exp $

%This function caluculates a vector for the FTRAC Bbs. This vector is used
%to 'correct' some of the errors in BB segmentation done from CT images.
%The points are then projected on this vector to decresae soem error. Error
%in the direction of this vector is not corrected. The vector is computed
%by a nonlinear optimization process.
function err_vec = FTRAC_Orientation_Error_Function(direction, FTRAC_Points)

direction = direction/norm(direction);
err_vec = [];

mn_pt = mean(FTRAC_Points(1:4,:));
for counter = 1 : 4
    err_vec = [err_vec; norm(cross(mn_pt - FTRAC_Points(counter, :), direction'))];
end

mn_pt = mean(FTRAC_Points(5:7,:));
for counter = 5 : 7
    err_vec = [err_vec; norm(cross(mn_pt - FTRAC_Points(counter, :), direction'))];
end

mn_pt = mean(FTRAC_Points(8:9,:));
for counter = 8 : 9
    err_vec = [err_vec; norm(cross(mn_pt - FTRAC_Points(counter, :), direction'))];
end

%$Log: FTRAC_Orientation_Error_Function.m,v $
%Revision 1.1  2006/11/21 18:37:26  jain
%First version of the set of files which can compare Ca-rm based seed reconstructions to Ground Truth obtained from CT.
%This function currently needs both the seeds and the FTRAC fiducial to be visible in
%the CT frame. Will write and add a new one that just works with 'relative reconstruction
%error' computation - i.e. where a post-operative CT is taken of the patient. The
%starting function currently is Compute_Reconstruction_Error.
%
