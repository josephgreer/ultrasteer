%Outputs the projected pixels from the input points from a camera model.
function out_pixels = create_image_pixels(pixel_ratio, focal_len, origin, pts)

%calculate the camera projection model. Do the projection. Output the pixels.
Proj_xform = [-focal_len*pixel_ratio(1) 0 origin(1) ; 0 -focal_len*pixel_ratio(2) origin(2) ; 0 0 1];
new_pts = Proj_xform*pts(:,1:3)';
out_pixels = [round((new_pts(1,:)./new_pts(3,:))') round((new_pts(2,:)./new_pts(3,:))') pts(:,4)]; 
