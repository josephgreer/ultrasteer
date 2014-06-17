function [eline] = find_eline (pixel1,cam1_name, stereo_pair, refraction, isGraphicsOn)
% A funcion that returns the epipolar line in the corresponding camera 
% intput
% selected_pixel
% the name of the selected camera
% the stereo geometry describing the camera pair

% output:
% the epipolar line (format TBD)


%Ensure that the stereo_pair has the cameras set up correctly, otherwise
%either reverse the stereo pair or declare an error. 

%% Define the stereo and refraction geometry

gelHeight = refraction.gel_height;
refIndex = refraction.ref_index;
gelNormal = refraction.gel_normal;


%% Initialize variables
cam1 = []; cam2 = [];
R_cam12world = []; T_cam12world =[]; G_cam12world = [];
R_world2cam2 = []; T_world2cam2 =[]; G_world2cam2 = [];
T_cam22world = [];
    
%% Project the pixels to the gel
% Use the selected pixel in the camera to generate a ray to the surface of
% the gel (at point p); form a ray that is // to the ray from the point on
% the gel to the COP

if (strcmp(cam1_name, stereo_pair.left_name))
    % Select the source camera
    cam1 = stereo_pair.left;
    R_cam12world = stereo_pair.R_left2world;
    T_cam12world = stereo_pair.T_left2world;
    G_cam12world = stereo_pair.G_left2world;
    
    % Select the destination camera
    cam2 = stereo_pair.right;
    R_world2cam2 = stereo_pair.R_world2right;
    T_world2cam2 = stereo_pair.T_world2right;
    G_world2cam2 = stereo_pair.G_world2right;
    T_cam22world = stereo_pair.T_right2world;
   
    
elseif (strcmp(cam1_name, stereo_pair.right_name))
    % Select the source camera
    cam1 = stereo_pair.right;
    R_cam12world = stereo_pair.R_right2world;
    T_cam12world = stereo_pair.T_right2world;
    G_cam12world = stereo_pair.G_right2world;
    
    % Select the destination camera
    cam2 = stereo_pair.left;
    R_world2cam2 = stereo_pair.R_world2left;
    T_world2cam2 = stereo_pair.T_world2left;
    G_world2cam2 = stereo_pair.G_world2left;
    T_cam22world = stereo_pair.T_left2world;
    
else
    Disp('ERROR: The source camera name for eline does not match either camera name in the stereo pair');
    stop
end

% Determine a vector in the direcion of the projection, the postion on
% the gel that the pixel is projected to, and a ray to the point
[x_1, p_1, ray_air_1] = project2gel(pixel1, R_cam12world, T_cam12world, G_cam12world, cam1.cc...
    , cam1.kc, cam1.alpha_c , gelHeight, cam1.pixel_width, mean(cam1.fc)/cam1.pixel_width,  isGraphicsOn); % /cam1.pixel_width


%% Using the projected ray, determine the refraction anglesa
[theta_air_1, theta_gel_1] = refractionAngles (ray_air_1, refIndex, gelNormal);
 

%% Generate a vector perpendicular to the surface normal yet in the plane
% of the incident ray
k_1 = genRefBasis (ray_air_1, gelNormal);


%% Take the incident on the gel and project it to camera 2
% Project the point on the gel formed by the inicident ray from the 1
% camera back onto the 2 camera
% This point in the image plane will be the stare of the epipolar line

epoint.top = point2image (p_1, R_world2cam2, T_world2cam2,cam2.fc/cam2.pixel_width, cam2.cc, cam2.kc, cam2.alpha_c); %/cam1.pixel_width

%% Determine the point in the bottom of the gel (or as low as we wish to
% consider

% Note: This is where only considering a height range of interest would be
% implemented

bottom_height = [0;0;0]; %bottom height above the world coordinate frame to be considered. % ANN CHANGED : originally [0;0;-5]

% Parameterize the ray projected in the gel
% construct a vector in the gel along the ray
directionVector = sin(theta_gel_1)*k_1 + cos(theta_gel_1)*gelNormal;

% Determine the distance along the directionVector to reach the bottom of
% the gel plus some extra margin
lineDistance.bottom = length2plane(p_1, directionVector, bottom_height, gelNormal );

%Calculate the bottom of the gel along the directionVector
endPoint_bottom = p_1 + lineDistance.bottom* directionVector;


%% Determine how this bottom point projects back to camera 2's image plane
% This will form the other end of the epipolar line

[epoint.bottom, surf_proj_bottom] = exit_refraction (endPoint_bottom, T_cam22world, gelNormal, gelHeight, refIndex,...
        R_world2cam2, T_world2cam2, cam2.fc/cam2.pixel_width, cam2.cc, [cam2.kc 0],cam2.alpha_c, isGraphicsOn );     %/cam1.pixel_width
        
eline = [epoint.top, epoint.bottom'];

