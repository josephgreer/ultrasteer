function [pts_2d,ors,trs,cams,pts_3d_truth]=sm_readdata_seeds_orientation(f_dir)
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

f_truth='struth.txt';%'ftruth.txt';%'groundtruth.txt';'gtruth.txt';
f_pt='seeds';%'seeds';%'segmented_pts_im_';'gs';'fid'
f_tr='pose';%'pose_im_';
f_cam='cam';%'camera_';
f_ang='ang';

pts_3d_truth=load(strcat(f_dir,f_truth));
%pts_3d_truth(:,3)=pts_3d_truth(:,3)-2.5;

fs=dir(strcat(f_dir,f_pt,'*'));
[n_ims,tmp]=size(fs);

pts_2d=cell(n_ims,1);
trs=cell(n_ims,1);
cams=cell(n_ims,1);
ors=cell(n_ims,1);

for i=1:n_ims
    pts_2d{i}=load(strcat(f_dir,f_pt,int2str(i),'.txt'));
    trs{i}=load(strcat(f_dir,f_tr,int2str(i),'.txt'));
    cams{i}=load(strcat(f_dir,f_cam,int2str(i),'.txt'));
    cams{i}(1)=1/cams{i}(1);
    cams{i}(2)=1/cams{i}(2);
    tmp=load(strcat(f_dir,f_ang,int2str(i),'.txt'));
    ors{i}=[cos(tmp),sin(tmp)];
end
