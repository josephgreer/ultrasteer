% $Id: Estimate_Pose.m,v 1.3 2005/12/28 02:44:11 jain Exp $

%The main function that estimates the pose of the fiducial, given segmented
%images. The features data in the model coordinate frame are given in the
%variables model_points for points; model_line_pts for the end points lying on the
%line and that define the line; model_ellipse1_pts and model_ellipse2_pts
%for points on the two ellipse. The segmented image information is supplied
%for as image_points for the points; image_line<i>_pts for the points that
%define the straight lines; image_ellp<i>_pts for points that define the
%ellipse in the image. Note that for the sake of point features, the
%corresponding model and image points have to match. For the sake of the
%line features, the corresponding lines are the matching lines and
%similarly for the ellipses too. The correspondence problem is not solved
%here. The camera parameters and an initial estimate are also provided. The
%pose of the fiducial, the final average error for each feature, the
%jacobian, individual errors and the condition number of the final
%jacobians are returned. In InputType1 pose estimation the details of the
%segmented fiducial in the image are supplied as points. The equation
%function [Pose, Error] = Estimate_Pose(model_points, image_points, model_line_pts, image_line1_pts, image_line2_pts, image_line3_pts, model_ellipse1_pts, model_ellipse2_pts, image_ellip1_pts, image_ellip2_pts, pixel_ratio, screen_size, origin, focal_len, initial_estimate)
function [Pose, Error] = Estimate_Pose(model_points, image_points, model_line_pts, image_lines, model_ellipse1_pts, model_ellipse2_pts, image_ellipses, pixel_ratio, screen_size, origin, focal_len, initial_estimate)

%An initial pose is estimated by running the pose estimation multiple
%number of times with various starting points around the initial estimate.
%The returned guess of the pose with minimum final error is taken as the
%best guess. Only the point features are used for this, since they are most
%stable to converge. Then taking this new estimate as a starting point, all
%the features are used to estimate the final pose. A preset number of
%iterations are run with various starting conditions and again the returned
%pose with minimum error is chosen. This is the final pose. To check
%divergence, an average error of 10mm is used as a threshold.
num_base_iterations = 25;
max_avg_err = 2.5; %orig 2.5

%Put all the input data together. 50 iterations for each starting point is
%sufficient.
model_points_p = model_points;
image_points_p = image_points;
model_line_pts_p = model_line_pts;
image_lines_p = image_lines; 
model_ellipse_pts_p = [model_ellipse1_pts ; model_ellipse2_pts];
num_model_ellipse_pts_p = [size(model_ellipse1_pts,1) ; size(model_ellipse2_pts,1)];
image_ellipse_p = image_ellipses;
maximum_iter = 50;

%Look at the two ellipses and discard one ellipse if it is extremely
%skewed (in that case it would be unreliable and error prone). If the ratio
%of majoraxis/minoraxis < 10 or so, it is a fine ellipse. According to the
%case, change the ellipse data that would be passed on to the pose
%estimation function in the end (after a good guess is obtained by 
%running pose estimation for just the point features).
sel_ellps = Ellipse_Check(image_ellipses);
if(prod(size(sel_ellps))==0)
    model_ellipse_pts_p = [];
elseif(prod(size(sel_ellps))==1)
    if(sum(sel_ellps==1)==1)
        model_ellipse_pts_p = [model_ellipse1_pts];
    elseif(sum(sel_ellps==2)==1)
        model_ellipse_pts_p = [model_ellipse2_pts];
    else
        display 'Error in choosing Ellipse Model Points'
    end
elseif(prod(size(sel_ellps))==2)
    if(sum(sel_ellps==[1 2]')==2)
        model_ellipse_pts_p = [model_ellipse1_pts; model_ellipse2_pts];
    elseif(sum(sel_ellps==[2 1]')==2)
        model_ellipse_pts_p = [model_ellipse2_pts; model_ellipse1_pts];
    else
        disp('Error in choosing Ellipse Model Points');
    end
else
    disp('Error in choosing Ellipse Model Points');
end
num_model_ellipse_pts_p = num_model_ellipse_pts_p(sel_ellps);
image_ellipse_p = image_ellipse_p(sel_ellps, :);

%Run the first set of iterations using only the point-features. The
%starting guess for each iteration covers the whole space so that the
%probability of missing the correct pose is minimised. The error in each
%case is noted...and a failure is also reported. A good guess for the next
%bulk of analysis is obtained. If 0 convergence is noticed, then Pose is
%set to -1.
failures = 0;
counter = 0;
good_transforms = [];
good_errors = [];
good_iter = [];
for count_times = 1:num_base_iterations
    start_guess = initial_estimate*xyz_xform(pi*(rand(3,1).*sign((rand(3,1)-0.5))), 100*rand(3,1).*sign((rand(3,1)-0.5)));
    [T error] = Find_Solution(model_points_p, image_points_p, [], [], [], [0;0], [], pixel_ratio, screen_size, origin, focal_len, start_guess, maximum_iter, 0);
    [num_iter fin_err] = Is_Converged(T, error, max_avg_err, focal_len);
    if(num_iter<0)
        failures = failures + 1;
    else
        counter = counter+1;
        good_transforms(:,:,counter) = T;
        good_errors(counter) = fin_err;
        good_iter(counter) = num_iter;
    end
end
if(counter==0)
    disp(['Failed badly! Cannot find a good INITIAL GUESS for Pose with less than 2.5mm error (at each of the feature Points). Try increasing the max_avg_err to greater than 2.5mm error.']);
    Pose = [];
    Error = [];
    return;
elseif(counter<0.1*num_base_iterations)
    display 'Less than 10% convergences for 6Points input, while trying to get a good starting estimate';
end
good_estimate = Choose_Transform(good_transforms, good_errors');

%Now use this estimate to start with. Run multiple iterations with
%different starting points near this good guess. The volume of initial 
%estimates is limited when compared to the previous set of 
%iterations. The number of iterations is also decreased. *All* the features
%are used in this case (except if an ellipse is extremely skewed). The
%whole procedure is similar to what was done above. The answer with minimum
%error is chosen as the final pose. If no transformation is obtained, then
%an empty matrix is returned.
failures = 0;
counter = 0;
good_transforms = [];
good_errors = [];
good_iter = [];
for count_times = 1:0.5*num_base_iterations
    start_estimate = good_estimate*xyz_xform(pi*0.05.*(rand(3,1).*sign((rand(3,1)-0.5))), [25; 25; 50].*rand(3,1).*sign((rand(3,1)-0.5)));
    [T error all_errs] = Find_Solution(model_points_p, image_points_p, model_line_pts_p, image_lines_p, model_ellipse_pts_p, num_model_ellipse_pts_p, image_ellipse_p, pixel_ratio, screen_size, origin, focal_len, start_estimate, maximum_iter, 0);
    [num_iter fin_err] = Is_Converged(T, error, max_avg_err, focal_len);
    if(num_iter<0)
        failures = failures + 1;
    else
        counter = counter+1;
        good_transforms(:,:,counter) = T;
        good_errors(counter) = fin_err;
        good_iter(counter) = num_iter;
        good_all_errs(:,:,counter) = all_errs;
    end
end
if(counter==0)
    display 'Failed! Cannot find any POSE with less than 2.5mm error (for each of the feature Points). Try increasing the max_avg_err to greater than 2.5mm.';
    Pose = [];
    Error = [];
else
    [Pose tmp index] = Choose_Transform(good_transforms, good_errors');
    Error = squeeze(good_all_errs(:,end,index));
end

% $Log: Estimate_Pose.m,v $
% Revision 1.3  2005/12/28 02:44:11  jain
% Some small changes in how comments are written. Also Bad nitial estimate was returning -1 for pose instead of []. One things that can be added is that maybe we don't need ellipses to be segmented - as far a the curve is a conic.
%
% Revision 1.2  2005/07/08 20:51:05  jain
% Estimate_Pose: Added CVS tags for Id and Log
%
