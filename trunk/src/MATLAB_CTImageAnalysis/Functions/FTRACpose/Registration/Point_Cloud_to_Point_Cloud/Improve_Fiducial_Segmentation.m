%$Id: Improve_Fiducial_Segmentation.m,v 1.1 2006/11/21 18:37:26 jain Exp $

%This function corrects for errors in BB segmentation from CT. A vector is used
%to 'correct' some of the errors in BB segmentation done from CT images.
%The points are then projected on this vector to decresae soem error. Error
%in the direction of this vector is not corrected. The vector is computed
%by a nonlinear optimization process. The input points are in the same
%order of correspondence as used through out the various modules.
function Corrected_Points = Improve_Fiducial_Segmentation(Raw_Segmented_Points)

Num_Iters = 10000;
options = optimset('Display', 'off', 'LargeScale', 'off', 'LevenbergMarquardt', 'on', 'MaxFunEvals', Num_Iters, 'MaxIter', Num_Iters);
initial_x = [ -0.00877816327011  -0.00311761530146   0.99999872371288]';
final_dir = lsqnonlin('FTRAC_Orientation_Error_Function', initial_x, [], [], options, Raw_Segmented_Points);

mn_pt = mean(Raw_Segmented_Points(1:4,:));
Corrected_Points(1:4,:) = repmat(mn_pt, 4, 1) + dot((Raw_Segmented_Points(1:4,:)' - repmat(mn_pt', 1, 4)), repmat(final_dir, 1, 4) )'*final_dir';

mn_pt = mean(Raw_Segmented_Points(5:7,:));
Corrected_Points(5:7,:) = repmat(mn_pt, 3, 1) + dot((Raw_Segmented_Points(5:7,:)' - repmat(mn_pt', 1, 3)), repmat(final_dir, 1, 3) )'*final_dir';

mn_pt = mean(Raw_Segmented_Points(8:9,:));
Corrected_Points(8:9,:) = repmat(mn_pt, 2, 1) + dot((Raw_Segmented_Points(8:9,:)' - repmat(mn_pt', 1, 2)), repmat(final_dir, 1, 2) )'*final_dir';

%$Log: Improve_Fiducial_Segmentation.m,v $
%Revision 1.1  2006/11/21 18:37:26  jain
%First version of the set of files which can compare Ca-rm based seed reconstructions to Ground Truth obtained from CT.
%This function currently needs both the seeds and the FTRAC fiducial to be visible in
%the CT frame. Will write and add a new one that just works with 'relative reconstruction
%error' computation - i.e. where a post-operative CT is taken of the patient. The
%starting function currently is Compute_Reconstruction_Error.
%

