function pts_2d = read_seeds(f_dir)


% pts_2d - cell array of images 

f_pt='seeds';%'seeds';%'segmented_pts_im_';'gs';'fid' seed1.txt seeds2.txt etc all three that follow are like this

fs = dir(strcat(f_dir,f_pt,'*'));
[n_ims,tmp] = size(fs)

pts_2d=cell(n_ims,1);

for i=1:n_ims
    pts_2d{i}=load(strcat(f_dir,f_pt,int2str(i),'.txt'));
end
