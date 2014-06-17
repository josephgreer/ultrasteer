%Function that simulates the pose estimation for a randomly chosen pose of
%the fiducial. The parameters/constants are defined inside...and the variable can be
%used for changing any parameter. A decent pose is randomly chosen and
%checked if it lies in the image dimensions. If it does, then that pose is
%chosen as permanent and simulated images are generated. Appropriate noise is added to 
%the simulated images. This data is used to run the pose estimation and
%results compared to the known ground truth. The error is returned to the
%calling function.
%[net_err, J, D, cnum] = Simulate(depth)
function [net_err1, pose_err1, net_err2, pose_err2]= Simulate(seg_err)

%The various parameters/constants.
D = 1.2*25.4;
th = 30*pi/180;

pixel_ratio = [4 4];
screen_size = [300 300];
origin = [600, 600];
focal_len = -1000;
fid_depth = 650;

%seg_err = 0.25;
cone_angle = pi/6*rand;

%To generate a random pose, the fluoroscope source is forced to be on a
%cone centered at the fiducial. The x,y,z-axis of the fiducial always are
%at a fixed location, i.e. only the x-ray is moved along a cone. The
%cone_pose function returns the transformation needed for this. 
%It is checked if the fiducial lies completely within acceptable bounds of
%the image. If it is, then that pose is acceptable...or the computation
%repeated. The simulated image data (position of the pixels, etc...) is
%noted in various variables by the segmentation_Results function.
%Appropriate error is added to each of that by the add_error function.
pose_acceptable = 0;
while (pose_acceptable==0)   
    actual_pose = cone_pose(cone_angle, 2*pi*rand(1), (pi/18)*rand(1)*sign(rand(1)-0.5), [0 0 -fid_depth]' + [50 ; 50 ; 100].*rand(3,1).*sign((rand(3,1)-0.5)));
    [model_pts model_line_pts model_ellipse1_pts model_ellipse2_pts] = Fiducial_points(D, th);
    [image_pts, image_line1_pts, image_line2_pts, image_line3_pts, image_ellipse1_pts, image_ellipse2_pts] = Segmentation_Results(actual_pose, D, th, pixel_ratio, screen_size, origin, focal_len);
    image_pts = add_error(image_pts, seg_err);
    image_line1_pts = add_error(image_line1_pts, seg_err);
    image_line2_pts = add_error(image_line2_pts, seg_err);
    image_line3_pts = add_error(image_line3_pts, seg_err);
    image_ellipse1_pts = add_error(image_ellipse1_pts, seg_err);
    image_ellipse2_pts = add_error(image_ellipse2_pts, seg_err);
    all_points = [image_pts; image_line1_pts; image_line2_pts; image_line3_pts; image_ellipse1_pts; image_ellipse2_pts];
    pose_acceptable = Draw_Points(all_points, screen_size, pixel_ratio, 0);
end

%Add calibration error to each of the camera parameters.
pixel_ratio = 1./((1./pixel_ratio) + 0.0005*rand(1,2).*sign(rand(1,2) - 0.5));
origin = add_error(origin, 2);
focal_len = focal_len + 2*rand(1).*sign(rand(1)-0.5);

%Get the equations of the lines in the image and stack them together.
image_line1 = least_squares_line(image_line1_pts);
image_line2 = least_squares_line(image_line2_pts);
image_line3 = least_squares_line(image_line3_pts);
image_lines = [image_line1 ; image_line1 ; image_line2 ; image_line2 ; image_line3 ; image_line3];

%Ge the equations of the ellipses in the image and stack them together.
image_ellipse1 = (fit_ellipse(image_ellipse1_pts));
image_ellipse2 = (fit_ellipse(image_ellipse2_pts));
image_ellipses = [image_ellipse1 ; image_ellipse2];

%Make an intial guess. Call the pose estimation function with the initial
%guess, camera parameters and segmentaed data. This is with all the
%features.
initial_guess = actual_pose*xyz_xform(0.5*pi*(rand(3,1).*sign((rand(3,1)-0.5))), 100*rand(3,1).*sign((rand(3,1)-0.5)));
[pose1, pose_err1] = Estimate_Pose(model_pts, image_pts, model_line_pts, image_lines, model_ellipse1_pts, model_ellipse2_pts, image_ellipses, pixel_ratio, screen_size, origin, focal_len, initial_guess);

%Check if there was a failure...if not, then measure the error.
if(prod(size(pose1))==0)
    net_err1 = [];
else
%    xform_err = inv(actual_pose)*pose;
%    err_tras = abs(xform_err(1:3,4));
%    err_rota = acos(sum(xform_err(1:3,1:3).*eye(3)))*180/pi;
%    net_err = [err_tras' err_rota];
    err_tras1 = abs(actual_pose(1:3,4) - pose1(1:3,4));
    err_rota1 = acos(sum(actual_pose(1:3,1:3).*pose1(1:3,1:3)))*180/pi;
    net_err1 = [err_tras1' err_rota1];
end

%Make an intial guess. Call the pose estimation function with the initial
%guess, camera parameters and segmentaed data. This is only with the
%points.
initial_guess = actual_pose*xyz_xform(0.5*pi*(rand(3,1).*sign((rand(3,1)-0.5))), 100*rand(3,1).*sign((rand(3,1)-0.5)));
[pose2, pose_err2] = Estimate_Pose(model_pts, image_pts, [], [], [], [], [], pixel_ratio, screen_size, origin, focal_len, initial_guess);

%Check if there was a failure...if not, then measure the error.
if(prod(size(pose2))==0)
    net_err2 = [];
else
%    xform_err = inv(actual_pose)*pose;
%    err_tras = abs(xform_err(1:3,4));
%    err_rota = acos(sum(xform_err(1:3,1:3).*eye(3)))*180/pi;
%    net_err = [err_tras' err_rota];
    err_tras2 = abs(actual_pose(1:3,4) - pose2(1:3,4));
    err_rota2 = acos(sum(actual_pose(1:3,1:3).*pose2(1:3,1:3)))*180/pi;
    net_err2 = [err_tras2' err_rota2];
end