%$Id: CT_to_FTRAC_Registration.m,v 1.1 2006/11/21 18:37:26 jain Exp $

%Function to compute the transform from CT to the FTRAC frame of reference.
%The input is the FTRAC BB locations wrt the CT frame and an initial
%estimate (3 euler angles, 3 translations - together as a 6x1 vector).
function [xform_CT_to_FTRAC, final_parameters] = CT_to_FTRAC_Registration(Points_wrt_CT, initial_estimate)

% figure
% plot3(Points_wrt_CT(:,1), Points_wrt_CT(:,2), Points_wrt_CT(:,3), 'r.')
% hold on
% Points_wrt_CT = Improve_Fiducial_Segmentation(Points_wrt_CT);
% plot3(Points_wrt_CT(:,1), Points_wrt_CT(:,2), Points_wrt_CT(:,3), 'g.')
% pause;

%The various parameters/constants for the fiducial.
D = 1.2*25.4;
th = 30*pi/180;
Points_wrt_FTRAC = Fiducial_points(D, th);

%Optimization parameters
Num_Iters = 50000;
options = optimset('Display', 'iter', 'LargeScale', 'off', 'LevenbergMarquardt', 'on', 'MaxFunEvals', Num_Iters, 'MaxIter', Num_Iters);
final_parameters = lsqnonlin('Point_Cloud_Registration_Error_Function', initial_estimate, [], [], options, Points_wrt_CT, Points_wrt_FTRAC);
xform_CT_to_FTRAC = xyz_xform(final_parameters(1:3), final_parameters(4:6));

% Plot for residual errors and visualization of both
% clouds after registration 
% figure, plot(Point_Cloud_Registration_Error_Function(xform_CT_to_FTRAC, Points_wrt_CT, Points_wrt_FTRAC))
% grid on
% tmp = Points_wrt_FTRAC;
% figure, plot3(tmp(:,1), tmp(:,2), tmp(:,3), 'go')
% hold on
% tmp = xform_CT_to_FTRAC*[Points_wrt_CT' ; ones(1, length(Points_wrt_CT))];
% tmp = tmp(1:3, :)';
% plot3(tmp(:,1), tmp(:,2), tmp(:,3), 'r*')
% grid on

%$Log: CT_to_FTRAC_Registration.m,v $
%Revision 1.1  2006/11/21 18:37:26  jain
%First version of the set of files which can compare Ca-rm based seed reconstructions to Ground Truth obtained from CT.
%This function currently needs both the seeds and the FTRAC fiducial to be visible in
%the CT frame. Will write and add a new one that just works with 'relative reconstruction
%error' computation - i.e. where a post-operative CT is taken of the patient. The
%starting function currently is Compute_Reconstruction_Error.
%

