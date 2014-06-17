%This function returns the projected image coordinates of the input
%fiducial points. All the camera parameters and the input data is to be
%provided.
function [projected_pixels] = get_projected_pixels(model_2_camera_xform, model_points, pixel_ratio, screen_size, origin, focal_len)

%Error check.
num_points = size(model_points, 1);
if(num_points==0)
    projected_pixels = [];
    return;
end

%The projective transformation.
camera_matrix = [focal_len*pixel_ratio(1)  0                        origin(1) 0 ;
                 0                         focal_len*pixel_ratio(2) origin(2) 0 ;
                 0                         0                        1         0];

%Fiducial points in the camera frame.
camera_homo_points = model_2_camera_xform*[model_points' ; ones(1, num_points)];
camera_points = (camera_homo_points(1:3,:)./(ones(3,1)*camera_homo_points(4,:)))';

%Projected image coordinates.
projected_homo_points = camera_matrix*camera_homo_points;
projected_pixels = [ (projected_homo_points(1,:)./projected_homo_points(3,:))' (projected_homo_points(2,:)./projected_homo_points(3,:))' ];

