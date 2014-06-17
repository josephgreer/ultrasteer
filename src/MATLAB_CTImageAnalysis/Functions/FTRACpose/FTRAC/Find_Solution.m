
%Function to calculate the pose of the fiducial given one specific choice
%of fiducial features and their projection in the image. The camera
%parameters are also passed to this function, and so is the number of
%iterations. It returns the final transform from the fiducial to the X-ray
%source, all types of error values and the jacobian of the last iteration.
%'D' is all the error values put together. If flag is set to 1, then the
%first 5 iterations of the fiducial in the image are displayed.

function [Transform, err, all_errs, J] = Find_Solution(model_points, image_points, line_pts_model, image_lines, ellipse_pts_model, num_model_ellipse_pts, image_ellipse, pixel_ratio, screen_size, origin, focal_len, initial_sol, num_iter, flag)

%Run the number of iterations of the newton's method for optimization.
updated_val = initial_sol;
for count = 2:num_iter+1
    [updated_val(:,:,count) err(count-1) all_errs(:,count-1) Jac(:,:,count-1)] = Next_Pose(flag*(count >5 & count<10), updated_val(:,:,count-1), model_points, image_points, line_pts_model, image_lines, ellipse_pts_model, image_ellipse, num_model_ellipse_pts, pixel_ratio, screen_size, origin, focal_len);
end

%Choose the approptiate values to be returned.
Transform = updated_val(:,:,count);
J = squeeze(Jac(:,:,end));
