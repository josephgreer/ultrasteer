function [pts_2d,pts_3d_truth,n_ims]=sm_readdata_seeds(f_dir)
%,pts_3d_truth
%sm_readdata reads the data from a data set.
%
%[pts_2d,trs,pts_3d_truth]=sm_readdata(f_dir), 
%where f_dir is the path of the directory of data set.
%sm_readdata returns pts_2d as a cell of image data (pts_2d contains
%K matrices corresponding to K images respectively, each matrix is 
%N*2 which corresponds to N pixels.),
%trs as a cell of transforms (trs contains K transforms corresponding
%to K images respectively, each transfrom is a 4*4 matrix.),
%pts_3d_truth as a N*3 matrix of the 3d positions of truly existing points.

% pts_2d - cell array of images 
% trs - cell array of transformations for each images (relative pose of the c-arm pose with respect to the fiducial
% cams - cell array of camera parameteres
% pts_3d_truth - 3d ground truths of the 3d locations of the seeds

f_truth='struth.txt';%'ftruth.txt';%'groundtruth.txt';'gtruth.txt';
f_pt='seeds';%'seeds';%'segmented_pts_im_';'gs';'fid' seed1.txt seeds2.txt etc all three that follow are like this
f_tr='pose';%'pose_im_';
f_cam='cam';%'camera_';

pts_3d_truth=load(strcat(f_dir,f_truth));
%pts_3d_truth(:,3)=pts_3d_truth(:,3)-2.5;

fs=dir(strcat(f_dir,f_pt,'*'));
[n_ims,tmp]=size(fs);

pts_2d=cell(n_ims,1);
% trs=cell(n_ims,1);
% cams=cell(n_ims,1);

for i=1:n_ims
    pts_2d{i}=load(strcat(f_dir,f_pt,int2str(i),'.txt'));
%     trs{i}=load(strcat(f_dir,f_tr,int2str(i),'.txt'));
%     cams{i}=load(strcat(f_dir,f_cam,int2str(i),'.txt'));
%     cams{i}(1)=1/cams{i}(1);
%     cams{i}(2)=1/cams{i}(2);
%    cams{i}(5)=-cams{i}(5);
end
