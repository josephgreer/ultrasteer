function Pose = eval_pose(f_dir,img_ref,cams, screen_size)

param_dir = strcat(f_dir, 'FTRACParameters\');
pose_dir = strcat(f_dir, 'ComputedPoses\');

st = int2str(img_ref);
while(prod(size(st))~=5)
    st = strcat('0', st);
end

seg_pts = load(strcat(param_dir, 'Segmented_pts_IM_', st, '.txt'));
seg_lns = load(strcat(param_dir, 'Segmented_lns_IM_', st, '.txt'));
seg_els = load(strcat(param_dir, 'Segmented_els_IM_', st, '.txt'));

seg_lns = [seg_lns(1,:) ; seg_lns(1,:) ; seg_lns(2,:) ; seg_lns(2,:) ;seg_lns(3,:) ; seg_lns(3,:)];

%No need since we flipped the axis
%seg_pts = [seg_pts(:,1) 480-seg_pts(:,2)];
%seg_lns = [seg_lns(:,1) -seg_lns(:,2) (seg_lns(:,3) + seg_lns(:,2)*480)];
%seg_els = [seg_els(:,1) -seg_els(:,2) seg_els(:,3) (seg_els(:,4)+480*seg_els(:,2)) (-seg_els(:,5) - 960*seg_els(:,3)) (seg_els(:,6) + 480*seg_els(:,5) + seg_els(:,3)*480*480)];
%seg_els = seg_els./(-seg_els(:,6)*ones(1,6));

%The various parameters/constants.
D = 1.2*25.4;
th = 30*pi/180;
[model_pts model_line_pts model_ellipse1_pts model_ellipse2_pts] = Fiducial_points(D, th);

%Distorted case.
% pixel_ratio = [3.234 3.083];
% screen_size = [1280 960];
% origin = [647.1188, 483.2224];
% focal_len = -1050;

% camera parameters
pixel_ratio = [cams{img_ref,1}(1)^-1 cams{img_ref,1}(2)^-1];
origin = [cams{img_ref,1}(3), cams{img_ref,1}(4)];
focal_len = -cams{img_ref,1}(5);
% screen_size

%undistorted case. pixel ration by using major axis, minor axiis, avg and minor with 254.
%pixel_ratio = [2.268 2.268];
%pixel_ratio = [2.137 2.137];
%pixel_ratio = [2.202 2.202];
%pixel_ratio = [2.103 2.103];
%screen_size = [480 480];
%origin = [227.72, 202.27];
%focal_len = -1157;

initial_estimate = [ 0  0 -1 0; 
                     1  0  0 0;
                     0 -1  0 -700;
                     0  0 0 1];
                 
[Pose, Error] = Estimate_Pose(model_pts, seg_pts, model_line_pts, seg_lns, model_ellipse1_pts, model_ellipse2_pts, seg_els, pixel_ratio, screen_size, origin, focal_len, initial_estimate);
% Pose
% figure, plot(Error)
% 
% save(strcat(pose_dir, 'Pose_IM_', st, '.txt'), 'Pose', '-ascii');
% save(strcat(pose_dir, 'Error_IM_', st, '.txt'), 'Error', '-ascii');
