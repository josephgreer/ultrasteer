%A function that simulates the segmentation function. The actual final
%segmentation function will take an image as input and give the pixel
%coordinates of the features. This function here takes all the parameters
%of the camera and the pose of the actual fiducial to calculate the pixel
%coordinates of the features. 
function [image_pts, image_line1_pts, image_line2_pts, image_line3_pts, image_ellipse1_pts, image_ellipse2_pts] = Segmentation_Results(actual_pose, D, th, pixel_ratio, screen_size, origin, focal_len)

%The fiducial points
[model_pts, model_line_pts, model_e1_pts, model_e2_pts] = Fiducial_points(D, th);

%Get the coordinates of the point features.
points = model_pts;
image_pts = get_projected_pixels(actual_pose, points, pixel_ratio, screen_size, origin, focal_len);

%Get the coodinates of the 3 line features. p1, p2 are the end points of
%the line. 8 more points are injected in the middle.
p1 = model_line_pts(1,:);
p2 = model_line_pts(2,:);
points = [];
for count = 1:10
    points = [points ; p1 + ((p2-p1)/10)*count];
end
image_line1_pts = get_projected_pixels(actual_pose, points, pixel_ratio, screen_size, origin, focal_len);

p1 = model_line_pts(3,:);
p2 = model_line_pts(4,:);
points = [];
for count = 1:10
    points = [points ; p1 + ((p2-p1)/10)*count];
end
image_line2_pts = get_projected_pixels(actual_pose, points, pixel_ratio, screen_size, origin, focal_len);

p1 = model_line_pts(5,:);
p2 = model_line_pts(6,:);
points = [];
for count = 1:10
    points = [points ; p1 + ((p2-p1)/10)*count];
end
image_line3_pts = get_projected_pixels(actual_pose, points, pixel_ratio, screen_size, origin, focal_len);

%12 points from each ellipse are chosen and projected.
image_ellipse1_pts = get_projected_pixels(actual_pose, model_e1_pts, pixel_ratio, screen_size, origin, focal_len);
image_ellipse2_pts = get_projected_pixels(actual_pose, model_e2_pts, pixel_ratio, screen_size, origin, focal_len);
