function needle = triangulate_shaft (stereo, needle_centers_source, source_cam_name,...
    needle_image_match, match_camera_name, refraction, isGraphicsOn)
% This function accepts an image of the needle centers, an image of the
% needle to be matched with centers, and stereo parameters and returns the
% points along the needle shaft as well as correspondance metricies

%Check to see if we have to invert the stereo pair
%if strcmp( stereo.right_name , source_cam_name)
%    stereo = invertStereoPair(stereo);
%end

% Ensure that the stereo pair is appropriate
%if ~(strcmp( stereo.left_name , source_cam_name) && strcmp( stereo.right_name , match_camera_name))
%    disp('error with stereo pair, does not matched the provided names')
%    affdds
%end

%Triangulate the needle
[needle.points, needle.source_pixels, needle.match_pixels, needle.correspondance_metric] = ...
    triangulateNeedle(needle_centers_source,source_cam_name,...
    needle_image_match, match_camera_name, stereo, refraction, isGraphicsOn);

%Assume all points are valid.  They will be evaluated later
%needle.valid_points = zeros(size(needle.points(1,:)));

%% Determine which triangulated needle points are acceptable
%needle.valid_points = eval_fit_goodness(55,20,6,0,refraction.gel_height,...
%    needle.correspondance_metric(:,2)',needle.correspondance_metric(:,1)',needle.points);



