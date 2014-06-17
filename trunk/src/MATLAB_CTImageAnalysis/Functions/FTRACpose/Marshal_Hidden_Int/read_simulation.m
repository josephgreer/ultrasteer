function [pts_2d,trs,cams_all, pts_3d_truth] = read_simulation(f_dir, err)
%,pts_3d_truth
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

f_truth='3DSeedPositions.txt';%'ftruth.txt';%'groundtruth.txt';'gtruth.txt';
f_pt='image_seeds_';%'seeds';%'segmented_pts_im_';'gs';'fid' seed1.txt seeds2.txt etc all three that follow are like this
f_tr='image_transform_';%'pose_im_';

pts_3d_truth=load(strcat(f_dir,f_truth));
%pts_3d_truth(:,3)=pts_3d_truth(:,3)-2.5;

fs=dir(strcat(f_dir,f_pt,'*'));
[n_ims,tmp]=size(fs);

pts_2d=cell(n_ims,1);
trs=cell(n_ims,1);
cams=cell(n_ims,1);

pixel_ratio = [4 4];
screen_size = [1200 1200];
origin = [600 600];
% focal_len = 6310/6;
focal_len = 1000;

cams_all = {};
for i = 1:n_ims
    cams_all{i,1}(1) = 1/pixel_ratio(1);
    cams_all{i,1}(2) = 1/pixel_ratio(2);
    cams_all{i,1}(3) = origin(1);
    cams_all{i,1}(4) = origin(2);
    cams_all{i,1}(5) = focal_len;
    cams_all{i,1} = cams_all{i,1}';
end

for i=1:n_ims
    pts_2d{i}=load(strcat(f_dir,f_pt,int2str(i),'.txt'));
    
    num = size(pts_2d{i}, 1);

    th = 2*pi*rand(num, 1);
    dis = err.*rand(num, 1);
    pts_2d{i} = pts_2d{i} + [dis*4 dis*4].*[cos(th) sin(th)];
    
    
%     pts_2d{i} = pts_2d{i}(1:5, :);
    trs{i}=load(strcat(f_dir,f_tr,int2str(i),'.txt'));
end

% TAKE THIS OUT: randomize seeds
%     pts_2d{1} = pts_2d{1}(randperm(n_seeds),:);
%     pts_2d{2} = pts_2d{2}(randperm(n_seeds),:);
% pts_2d{1} = flipud(pts_2d{1})
% pts_2d{3} = flipud(pts_2d{3})
