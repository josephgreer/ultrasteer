function [needle_points, pixels_source, pixels_match, correspondance_metric] = ...
    triangulateNeedle (pixels_source, source_camera_name, match_image, match_camera_name, stereo_pair, refraction_params, isGraphicsOn)


%Input:
% The center-line points along the needle in the source camera
% The name of the source camera
% The pixels belonging to the needle in a match camera
% The match camera name 
% The stereo pair describing the transformation between the cameras

%Output:
% The 3-D points of the needle (in the world coordinate frame described by the stereo pair)
% The pixels matched to the source pixels
% A correspondance metric that describes the matches (1st col: # of pixels
% along the eline, 2nd col: angle btwn needle and eline)
%
% Athor: Tom Wedlick

%%

% Use the epipolar constraints to locate the matching pixels
[pixels_match, correspondance_metric] = find_corres_pix2  (pixels_source, source_camera_name,...
    match_image, match_camera_name, stereo_pair, refraction_params, isGraphicsOn);

%Use the pixel matches to triangulate the needle
% Note that pixels_match must be the right pixels and pixels left must be
% the source pixels.  See triangulate.m for clarity. 
needle_points = triangulate(pixels_match,pixels_source,stereo_pair,refraction_params);


if (isGraphicsOn)
end
