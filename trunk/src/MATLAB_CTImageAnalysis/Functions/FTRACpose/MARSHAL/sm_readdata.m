function [pts_2d,trs,pts_3d_truth]=sm_readdata(f_dir)
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

f_truth='3dseedpositions.txt';
f_pt='image_seeds_';
f_tr='image_transform_';

pts_3d_truth=load(strcat(f_dir,f_truth));

fs=dir(strcat(f_dir,f_pt,'*'));
[n_ims,tmp]=size(fs);

pts_2d=cell(n_ims,1);
trs=cell(n_ims,1);

for i=1:n_ims
    pts_2d{i}=load(strcat(f_dir,f_pt,int2str(i),'.txt'));
    trs{i}=load(strcat(f_dir,f_tr,int2str(i),'.txt'));
end
