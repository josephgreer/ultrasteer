function valid_points = eval_fit_goodness (angle_thresh, max_points, min_points, max_height, min_height,...
    angle_diff, point_number, point_locations)

%function valid_points = eval_fit_goodness (angle_thresh, max_points, min_points, max_height, min_height,...
%    angle_diff, point_number, point_locations)
% Accepts various measures of the goodness of the epipolar line to
% reconstruct the needle and returns a matrix where 1 corresponds to good
% points and a zero corresponds to poor points

% Initialize every points to be valid
%valid_points = ones(size(needle(pair).points));

% Perscribe a minimum angle that must exist between the e-line and the
% needle (in degrees)
valid_points = angle_diff >= angle_thresh;


% Perscribe a maximum and minumum number of needle pixels that can be
% on the epipolar line
valid_points = valid_points.* ( point_number <= max_points) .* (point_number >= min_points);


%Ensure that the triangulated points are inside the gel (roughly)
if ~(isempty(point_locations))
    valid_points = valid_points .* ( point_locations(3,:) < max_height ) .* (point_locations(3,:) > min_height );
else
    valid_points = [];
end

