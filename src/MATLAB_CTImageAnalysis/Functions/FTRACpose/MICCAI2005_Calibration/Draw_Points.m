%A function to check if the fiducial falls within acceptable bounds of the
%simulated image. If is does not, then that  particular pose is not good.
%the variable draw_fig if set to 1, will draws the fiducial in the image.
function pose_acceptable = Draw_Points(points, screen_size, pixel_ratio, draw_fig)

pix_dim = screen_size.*pixel_ratio;
num_pts = size(points,1);
upper_corner = 0.125*pix_dim;
lower_corner = 0.875*pix_dim;

if( sum(sum(points<ones(num_pts,1)*upper_corner))>0 )
    pose_acceptable = 0;
elseif(sum(sum(points>(ones(num_pts,1)*lower_corner)))>0)
    pose_acceptable = 0;
else
    pose_acceptable = 1;
end

if(pose_acceptable==1 & draw_fig==1)
    I = zeros(screen_size.*pixel_ratio);
    I(round(points(:,1)) + round(points(:,2)-1)*screen_size(1)*pixel_ratio(1)) = 1;
    I = conv2(I, fspecial('gaussian', 2, 1), 'same');
    figure, imshow(I>0);
end
    
