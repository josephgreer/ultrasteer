function [fc_left, cc_left, kc_left, alpha_c_left,...
    fc_right, cc_right, kc_right, alpha_c_right, om, T,R,...
    R_leftWorld, T_leftWorld, g_leftWorld,...
    R_wleft, T_wleft, g_wleft,...
    R_rightWorld, T_rightWorld, g_rightWorld,...
    R_wright, T_wright, g_wright] = unwrap_stereo_params (stereo)


% store the camera parameters to be passed back
fc_left = stereo.left.fc;
cc_left = stereo.left.cc;
kc_left = stereo.left.kc;
alpha_c_left = stereo.left.alpha_c;

fc_right = stereo.right.fc;
cc_right = stereo.right.cc;
kc_right = stereo.right.kc;
alpha_c_right = stereo.right.alpha_c;



% Store the steo parameters from the calibration toolbox to be passed back
stereo.left_name = 'left'; %should be the name of the left camera
stereo.right_name = 'right'; %should be the name of the right camera THIS SHOULD BE PASSED IN
om = stereo.om; %rotation  vector between right and left cameras (output of stereo calibration), position of right camera wrt left camera
T = stereo.T; %position of left camera in right camera coordinate frame
R = stereo.R;




% left_pix = stereo.left_pix;
% right_pix = stereo.right_pix;
% 
% calibration_file = stereo.calibration_file;
% % 
% 
R_leftWorld = stereo.R_world2left;
T_leftWorld = stereo.T_world2left;
g_leftWorld = stereo.G_world2left; %Homogeneous transform from world frame to the left frame

R_wleft = stereo.R_left2world;
T_wleft = stereo.T_left2world;
g_wleft = stereo.G_left2world;

R_rightWorld = stereo.R_world2right;
T_rightWorld = stereo.T_world2right;
g_rightWorld = stereo.G_world2right; 

R_wright = stereo.R_right2world;
T_wright = stereo.T_right2world;
g_wright = stereo.G_right2world;