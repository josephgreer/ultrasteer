%$Id: Compute_Seed_Locations_wrt_FTRAC.m,v 1.1 2006/11/21 18:37:26 jain Exp $

%Function computes the seed locations wrt the FTRAC fiducial frame.
%The input is the name of the directory where all the input
%data files are kept. Note that all the file formats wrt the data are fixed and
%hence loaded straight away - this aids us in not needing to pass lot of
%variables. For the FTRAC to CT optimization, if initial estimate has already been
%computed, just an old saved value is loaded - this allows the matlab version
%on the laptop to run this function. To run it with something else, just delete
%the 'xform_CT_to_FTRAC_initial_estimate.txt'  file in the directory.
function True_Seeds_wrt_FTRAC = Compute_Seed_Locations_wrt_FTRAC(Dir_Name)

%All the filenames to start with.
Filename_CT_Seeds = 'Seeds_wrt_CT.txt';
Filename_FTRAC_BBs = 'Fiducial_Points_wrt_CT.txt';
Filename_CT_to_FTRAC_xform = 'xform_CT_to_FTRAC.txt';
Filename_CT_to_FTRAC_initial_estimate = 'xform_CT_to_FTRAC_initial_estimate.txt';
Filename_True_Seeds_wrt_FTRAC = 'True_Seeds_wrt_FTRAC.txt';

%Load files from file
FTRAC_BBs_wrt_CT = load(strcat(Dir_Name, '/', Filename_FTRAC_BBs));
True_Seeds_wrt_CT = load(strcat(Dir_Name, '/', Filename_CT_Seeds));

% Do the point cloud to point cloud registration.
% If initial estimate is already computed, just load everything that was saved 
% before. If doing everything for
% the first time, then save all the relevant information
file_handle = fopen(strcat(Dir_Name, '/', Filename_CT_to_FTRAC_initial_estimate));
if(file_handle == -1)
    initial_estimate = [zeros(3,1); (mean(Points_wrt_FTRAC) - mean(Points_wrt_CT))'];
else
    fclose(file_handle);
    initial_estimate = load(strcat(Dir_Name, '/', Filename_CT_to_FTRAC_initial_estimate));
end
[xform_CT_to_FTRAC final_params] = CT_to_FTRAC_Registration(FTRAC_BBs_wrt_CT, initial_estimate);

%True (CT) and Computed (C-arm) seed locations with respect to the
%fiducial frame. 
True_Seeds_wrt_FTRAC = xform_CT_to_FTRAC*([True_Seeds_wrt_CT ones(size(True_Seeds_wrt_CT,1), 1)]');
True_Seeds_wrt_FTRAC = True_Seeds_wrt_FTRAC(1:3,:)';

%Save the values
save(strcat(Dir_Name, '/', Filename_CT_to_FTRAC_xform), 'xform_CT_to_FTRAC', '-ascii');
save(strcat(Dir_Name, '/', Filename_CT_to_FTRAC_initial_estimate), 'final_params', '-ascii');
save(strcat(Dir_Name, '/', Filename_True_Seeds_wrt_FTRAC), 'True_Seeds_wrt_FTRAC', '-ascii');

%$Log: Compute_Seed_Locations_wrt_FTRAC.m,v $
%Revision 1.1  2006/11/21 18:37:26  jain
%First version of the set of files which can compare Ca-rm based seed reconstructions to Ground Truth obtained from CT.
%This function currently needs both the seeds and the FTRAC fiducial to be visible in
%the CT frame. Will write and add a new one that just works with 'relative reconstruction
%error' computation - i.e. where a post-operative CT is taken of the patient. The
%starting function currently is Compute_Reconstruction_Error.
%
