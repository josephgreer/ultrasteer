function [res_ra,res_bpe,res_re,res_ren,averages,minmax] = compute_error(f_dir, pixel_ratio, screen_size, origin, focal_len)

ns_im =    [1,2,3;...
            1,2,4;...
            1,2,5;...
            1,2,6;...
            1,3,4;...
            1,3,5;...
            1,3,6;...
            1,4,5;...
            1,4,6;...
            1,5,6;...
            2,3,4;...
            2,3,5;...
            2,3,6;...
            2,4,5;...
            2,4,6;...
            2,5,6;...
            3,4,5;...
            3,4,6;...
            3,5,6;...
            4,5,6];
        
% read in data from files
[pts_2d_all,pts_3d_truth,n_ims] = sm_readdata_seeds(f_dir);

trs_all = {};
cams_all = {};

for i = 1:n_ims
    cams_all{i,1}(1) = 1/pixel_ratio(1);
    cams_all{i,1}(2) = 1/pixel_ratio(2);
    cams_all{i,1}(3) = origin(1);
    cams_all{i,1}(4) = origin(2);
    cams_all{i,1}(5) = focal_len;
    cams_all{i,1} = cams_all{i,1}';
end

% cams_all{3,1}(5) = focal_len + 10;

% for i = 1:n_ims
%    cams_all{i}(5) 
% end
% calculate pose
for i = 1:n_ims
    trs_all{i} = eval_pose(f_dir,i,cams_all, screen_size);
end

% calculate accuracy and errors
averages = zeros(4);
minmax = [inf(4,1),-inf(4,1)];

for i = 1:size(ns_im,1)
% for i = 1:1
    [res_ra,res_bpe,res_re,res_ren] = sm_main_seeds(f_dir,pts_2d_all,trs_all,cams_all,pts_3d_truth, ns_im(i,:));
    res_re
    res_ren
    averages = averages + [res_ra;res_bpe;res_re;res_ren];

    minmax(1,1) = min(minmax(1,1),res_ra(1));
    minmax(2,1) = min(minmax(2,1),res_bpe(1));
    minmax(3,1) = min(minmax(3,1),res_re(1));
    minmax(4,1) = min(minmax(4,1),res_ren(1));
    minmax(1,2) = max(minmax(1,2),res_ra(1));
    minmax(2,2) = max(minmax(2,2),res_bpe(1));
    minmax(3,2) = max(minmax(3,2),res_re(1));
    minmax(4,2) = max(minmax(4,2),res_ren(1));
end

averages = averages / size(ns_im,1);