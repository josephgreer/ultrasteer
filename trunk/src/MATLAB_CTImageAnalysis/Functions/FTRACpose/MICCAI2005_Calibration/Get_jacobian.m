%This is a function that computes the jacobian for input points on the
%fiducial, at any input specific location wrt the X-ray imager. For each
%point it computes how the projected point would behave with a variation in
%one of the 6 parameters. The two things varying in the image are the x and
%y coordinates of each projection. Thus the returned jacobian is num of
%points times 12 (6param x 2). The references can be sought in the paper
%and in Intro techniques for 3D Computer Vision by Trucco and Verri, pg
%283. There are some mistakes in the book that I corrected.
function J = Get_jacobian(model_2_camera_xform, model_points, pixel_ratio, screen_size, origin, focal_len)

%Calculate the number of input points, camera transformation and their
%projected positions in the image.
num_points = size(model_points, 1);
if(num_points==0)
    J = [];
    return;
end
camera_matrix = [focal_len*pixel_ratio(1)  0                        origin(1) 0 ;
                 0                         focal_len*pixel_ratio(2) origin(2) 0 ;
                 0                         0                        1         0];
camera_homo_points = model_2_camera_xform*[model_points' ; ones(1, num_points)];
camera_points = (camera_homo_points(1:3,:)./(ones(3,1)*camera_homo_points(4,:)))';

%Calculate the translations parts of the jacobian for the chosen model
%points. u represents the x-axis and v the y-axis. T1, T2, T3 represent the
%3 translation parameters, namely x,y&z-axis.
derivative_u_T1 = focal_len*pixel_ratio(1)./camera_points(:,3);
derivative_u_T2 = zeros(num_points,1);
derivative_u_T3 = -focal_len*pixel_ratio(1)*(camera_points(:,1)./(camera_points(:,3).*camera_points(:,3)));
derivative_v_T1 = zeros(num_points,1);
derivative_v_T2 = focal_len*pixel_ratio(2)./camera_points(:,3);
derivative_v_T3 = -focal_len*pixel_ratio(2)*(camera_points(:,2)./(camera_points(:,3).*camera_points(:,3)));

%In a similar way the rotation parts of the jacobian need to be computed.
%This reuires a little more work than direct substitution. A cross product
%needs to be computed. I don't write a closed form solution, but more of an
%algorithmic computable form.
derivative_u_angle = [];
derivative_v_angle = [];
for counter = 1:3
    vec = zeros(1,3);
    vec(counter) = 1;
    der_points_modelframe = -cross(ones(num_points,1)*vec, model_points);
    der_points_cameraframe = (model_2_camera_xform(1:3,1:3)*der_points_modelframe')';
    derivative_u_angle(:,counter) = focal_len*pixel_ratio(1)*((camera_points(:,3).*der_points_cameraframe(:,1) - camera_points(:,1).*der_points_cameraframe(:,3))./(camera_points(:,3).*camera_points(:,3)));
    derivative_v_angle(:,counter) = focal_len*pixel_ratio(2)*((camera_points(:,3).*der_points_cameraframe(:,2) - camera_points(:,2).*der_points_cameraframe(:,3))./(camera_points(:,3).*camera_points(:,3)));
end

%Put everything together abd return the jacobian.
J_u = [derivative_u_T1 derivative_u_T2 derivative_u_T3 derivative_u_angle];
J_v = [derivative_v_T1 derivative_v_T2 derivative_v_T3 derivative_v_angle];
J = [J_u; J_v];
