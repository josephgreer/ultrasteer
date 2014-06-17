%$Id: Compute_Reconstruction_Errors_without_Optimization.m,v 1.1 2006/11/21 18:37:26 jain Exp $

%The inputs are three. First two are the names of the directories where all the 
%input data files are kept. The third is the filename of the computed seeds that
%you want to compare to (this file name is also in ref to the data
%directory). Note that all the file formats wrt the data are fixed and
%hence loaded straight away - this aids us in not needing to pass lot of
%variables. Also, CT to FTRAC registration is not computed by optimization
%but by just loading the file. The files that you need in 'Ground_Truth_Dir_Name' are
%'Seeds_wrt_CT.txt', 'Fiducial_Points_wrt_CT.txt',
%'xform_FTRAC_to_Template.txt', 'True_Seeds_wrt_FTRAC.txt'.
function [Err, Err_Vecs] = Compute_Reconstruction_Errors_without_Optimization(Groud_Truth_Dir_Name, Carm_Seeds_Dir_Name, Filename_Carm_Computed_Seeds)

%All the filenames to start with.
Filename_FTRAC_to_Template_xform = 'xform_FTRAC_to_Template.txt';
Filename_True_Seeds_wrt_FTRAC = 'True_Seeds_wrt_FTRAC.txt';

True_Seeds_wrt_FTRAC = load(strcat(Groud_Truth_Dir_Name, '/', Filename_True_Seeds_wrt_FTRAC));
FTRAC_to_Template = load(strcat(Groud_Truth_Dir_Name, '/', Filename_FTRAC_to_Template_xform));
Template_to_FTRAC = inv(FTRAC_to_Template);    
Computed_Seeds_wrt_Template = load(strcat(Carm_Seeds_Dir_Name, '/', Filename_Carm_Computed_Seeds));
Computed_Seeds_wrt_FTRAC = Template_to_FTRAC*[Computed_Seeds_wrt_Template' ; ones(1, size(Computed_Seeds_wrt_Template, 1))];
Computed_Seeds_wrt_FTRAC = Computed_Seeds_wrt_FTRAC(1:3,:)';

% Compare these two clouds to see how well the they match.
[Err matching] = Point_Cloud_Registration_Error_Function(eye(4), Computed_Seeds_wrt_FTRAC, True_Seeds_wrt_FTRAC);
mean(Err)
Err_Vecs = Plot_3D_Clouds(True_Seeds_wrt_FTRAC, Computed_Seeds_wrt_FTRAC, matching);

%$Log: Compute_Reconstruction_Errors_without_Optimization.m,v $
%Revision 1.1  2006/11/21 18:37:26  jain
%First version of the set of files which can compare Ca-rm based seed reconstructions to Ground Truth obtained from CT.
%This function currently needs both the seeds and the FTRAC fiducial to be visible in
%the CT frame. Will write and add a new one that just works with 'relative reconstruction
%error' computation - i.e. where a post-operative CT is taken of the patient. The
%starting function currently is Compute_Reconstruction_Error.
%
