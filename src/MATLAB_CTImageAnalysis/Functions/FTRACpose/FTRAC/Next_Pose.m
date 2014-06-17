%Function that does the core work of pose estimation. It carries out a
%single iteration of the Newton's optimization method. It solves the equation
%J.x = D, for x. D is the distance (vector) value for each feature in the current
%projection. It is the signed difference in the X, Y coordinates for
%image points and projected fiducial points; signed distance from the image line to 
%the projected end-points on the line; and the signed distance of the (12) projected 
%points on each ellipse (on the fiducial) to the image ellipse. The closed
%form of the Jacobian is also known. Thus knowning J and D, x is computed
%and added to the original estimate. The rotation matrices and treated
%differently from the translation matrices in the last operation.
function [fiducial_2_fluoro_xform, err, all_errs, J] = Next_Pose(flag, present_xform, model_end_points, image_end_points, model_line_points, image_lines, ellipse_pts_model, ellipse_im, num_pts_ellipse, pixel_ratio, screen_size, origin, focal_len)

%Calculate J and D for the point features. D will consist of the X and Y
%coordinate, signed differences. The fiducial points are projected using 
%present transformation and the distance measured from the image points.
%If there are no points, then each quantity is null.
if(size(image_end_points, 1)~=0)
    J_point_corres = Get_jacobian(present_xform, model_end_points, pixel_ratio, screen_size, origin, focal_len);
    projected_end_pixels = get_projected_pixels(present_xform, model_end_points, pixel_ratio, screen_size, origin, focal_len);
    delta_u = projected_end_pixels(:,1) - image_end_points(:,1);
    delta_v = projected_end_pixels(:,2) - image_end_points(:,2);
else
    J_point_corres = [];
    delta_u = [];
    delta_v = [];
end

%Calculate J and D for the line features. Project the end points of the
%lines on the image using the currect transformation. Measure the distance
%of these points from the image lines. The distance formula is the typical
%substitution in the equation of the line. The jacobian can hence be
%calculated easily.
num_line_points = size(model_line_points, 1);
if(num_line_points~=0)
    J_line_points = Get_jacobian(present_xform, model_line_points, pixel_ratio, screen_size, origin, focal_len);
    J_u_line_points = J_line_points(1 : num_line_points, :);
    J_v_line_points = J_line_points(num_line_points+1 : 2*num_line_points, :);
    J_D_line_points = J_u_line_points.*(image_lines(:,1)*ones(1, 6)) + J_v_line_points.*(image_lines(:,2)*ones(1, 6));

    %Error
    projected_line_point_pixels = get_projected_pixels(present_xform, model_line_points, pixel_ratio, screen_size, origin, focal_len);
    delta_D = sum( image_lines.*[projected_line_point_pixels ones(num_line_points, 1)] , 2);
else
    J_D_line_points = [];
    delta_D = [];
end

%Calculate J and D for the ellipse features. Run a loop for each ellipse
%and append for J and D. The Distance funtion for each ellipse is a vector
%of length 12. 12 points on the fiducial surface (on the ellipse) are
%projected on the image using the current transformation. The distance of
%these 12 projected points from the image ellipse forms the vector. A scal
%factor is introduced to get these distances in the same order of magnitude
%as the line/point distances. The distance function before the scale factor
%is the normal 2D ellipse equation and the substitution of the point in it.
%The scale factor makes this close to the eucledian distance, for points near 
%the ellipse. The same equation easily also yield the jacobian.
num_ellipse = sum(num_pts_ellipse>0);

Jacobian_ellipse_points = [];
delta_D_ellp = [];
for count = 1 : num_ellipse
%Projected points for each ellipse.    
    this_Jacobian = [];
    this_Delta_D = [];
    ellipse_model = ellipse_pts_model(sum(num_pts_ellipse(1:(count-1)))+1:sum(num_pts_ellipse(1:count)) , :);
    ellipse_eq_im = ellipse_im(count,:);
    J_this_points = Get_jacobian(present_xform, ellipse_model, pixel_ratio, screen_size, origin, focal_len);
    projected_ellipse1_point_pixels = get_projected_pixels(present_xform, ellipse_model, pixel_ratio, screen_size, origin, focal_len);

%scale factor computation.
    A = [ellipse_eq_im(1) ellipse_eq_im(2)/2 ellipse_eq_im(4)/2 ; ellipse_eq_im(2)/2 ellipse_eq_im(3) ellipse_eq_im(5)/2 ; ellipse_eq_im(4)/2 ellipse_eq_im(5)/2 ellipse_eq_im(6)]; 
    B = A(1:2, 1:2);
    C = ellipse_eq_im(1) + ellipse_eq_im(3);
    a_dash = (C + sqrt( (ellipse_eq_im(1) - ellipse_eq_im(3))^2 + ellipse_eq_im(2)^2 ))/2;
    b_dash = (C - sqrt( (ellipse_eq_im(1) - ellipse_eq_im(3))^2 + ellipse_eq_im(2)^2 ))/2;
    c_dash = det(A)/det(B);
    major_axis = sqrt(-c_dash/a_dash);
    minor_axis = sqrt(-c_dash/b_dash);
    mult_const = major_axis*minor_axis/(2*norm([major_axis minor_axis])*abs(c_dash));

%Calculate the D values.    
    num_ellp_pts = size(projected_ellipse1_point_pixels, 1);
    tmp_u = projected_ellipse1_point_pixels(:,1);
    tmp_v = projected_ellipse1_point_pixels(:,2);
    this_delta_D = mult_const*[tmp_u.*tmp_u tmp_u.*tmp_v tmp_v.*tmp_v tmp_u tmp_v ones(num_ellp_pts,1)]*ellipse_eq_im';

%calculate the Jacobian column by column, i.e. parameter by parameter.
    tmp2 = [tmp_u tmp_v ones(num_ellp_pts,1)]*(2*A(:,1:2));
    this_Jacobian(:,1) = sum(tmp2.*[J_this_points(1:num_ellp_pts, 1) J_this_points(num_ellp_pts+1:2*num_ellp_pts, 1)], 2);
    this_Jacobian(:,2) = sum(tmp2.*[J_this_points(1:num_ellp_pts, 2) J_this_points(num_ellp_pts+1:2*num_ellp_pts, 2)], 2);
    this_Jacobian(:,3) = sum(tmp2.*[J_this_points(1:num_ellp_pts, 3) J_this_points(num_ellp_pts+1:2*num_ellp_pts, 3)], 2);
    this_Jacobian(:,4) = sum(tmp2.*[J_this_points(1:num_ellp_pts, 4) J_this_points(num_ellp_pts+1:2*num_ellp_pts, 4)], 2);
    this_Jacobian(:,5) = sum(tmp2.*[J_this_points(1:num_ellp_pts, 5) J_this_points(num_ellp_pts+1:2*num_ellp_pts, 5)], 2);
    this_Jacobian(:,6) = sum(tmp2.*[J_this_points(1:num_ellp_pts, 6) J_this_points(num_ellp_pts+1:2*num_ellp_pts, 6)], 2);
    this_Jacobian = mult_const*this_Jacobian;
    
    Jacobian_ellipse_points = [Jacobian_ellipse_points ; this_Jacobian];
    delta_D_ellp = [delta_D_ellp ; this_delta_D];
end

%If flag is set to 1, then display selected projected points. Choose the
%points to be displayed, get the projected coordinates, and show it on an
%image. Convolve to make it viewable.
if(flag~=0)
    I = zeros(screen_size);
    ptss = [model_end_points ; model_line_points ; ellipse_pts_model];
    propi = get_projected_pixels(present_xform, ptss, pixel_ratio, screen_size, origin, focal_len);
    for counter = 1:size(propi,1)
        if(round(propi(counter,1))>0 && round(propi(counter,1))<1200 && round(propi(counter,2))>0 && round(propi(counter,1))<1200)
            I( round(propi(counter,1)), round(propi(counter,2)) ) = 1;
        end
    end
    I = conv2(I, fspecial('gaussian', 3,2), 'same');
    figure, imshow((I>0));
end


%Form the full Jacobiand and the full error/distance matrices.
J = [J_point_corres ; J_D_line_points ; Jacobian_ellipse_points];
delta_F = [delta_u ; delta_v ; delta_D ; delta_D_ellp];

%J needs to be normalised so that any of the 6 parameters have the same
%sensitivity. Distance needs to be normalised so that each distance metric
%has the same weight. H normalises J; while G normalises D. The further
%mathematical references can be sought in the paper. The normalisation of H
%is with a 1mm displacement.
H = diag([0.03598305257216   0.02975478931761   0.31303227890921   0.00111529693836   0.00101967304364 0.00349752630978]);
std_pts = 1.45330739546944; 
std_lns = 1.05583140942755;
std_ellps = 0.67785843430350;
G = diag( [(1/std_pts)*ones(1, 2*size(image_end_points, 1)) (1/std_lns)*ones(1, size(model_line_points, 1)) (1/std_ellps)*ones(1, sum(num_pts_ellipse))] );

%Calculate the variable change using standard leastsquares. Update the
%current estimate of the transformation using this differential value. Note
%that there are three methods to update variables - 1)add the 6 parameter
%values 2)Update the complete frame transformation by combining this new
%transformation with the previous estimate 3)Add translations while
%combining the rotation transformations. Only the 3rd method was stable and 
%converged.
variable_change = H*mldivide(G*J*H, G*delta_F);
delt = delta_F;
all_errs = abs(delta_F)*mean((1./pixel_ratio));
err = mean(all_errs);

delta_F = xyz_xform(-variable_change(4:6), -variable_change(1:3)); 
fiducial_2_fluoro_xform = eye(4);
fiducial_2_fluoro_xform(1:3,4) = present_xform(1:3,4) + delta_F(1:3, 4);
fiducial_2_fluoro_xform(1:3,1:3) = present_xform(1:3,1:3)*delta_F(1:3, 1:3);
