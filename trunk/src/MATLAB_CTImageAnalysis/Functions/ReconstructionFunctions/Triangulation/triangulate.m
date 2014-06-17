function P_RefAccounted = triangulate(pix_right,pix_left, stereo_pair, refraction_params)
% % mod by Tom Wedlick
% previously FUNCTION: P_RefAccounted = Actualtriangulation(xR,yR,xL,yL)
%--------------------------------------------------------------------------
% function P_RefAccounted = triangulate(pix_right,pix_left, stereo_pair,
% refraction_params)
% Inputs: xR (scalar), yR (scalar), xL (scalar), yL (scalar) Point
%   coordinates from each camera. 
% Outputs: P_RefAccounted (3x1 vector) the triangulated point from xR,yR
%   and xL,yL.
% Sub_Functions Called: worldFrame.m, normalize(), stereo_triangulation()
% Synopsis:
%   This program will use triangulation to find the actual location of the
%   needle tip in the phantom tissue. MORE DETAILS?
%--------------------------------------------------------------------------

%%CAM C IS LEFT CAMERA N CAM B IS RIGHT CAMERA!!
%close all
%clear all
 
%load_camera_calibrations
[fc_left, cc_left, kc_left, alpha_c_left,...
    fc_right, cc_right, kc_right, alpha_c_right, om, T,R,...
    R_leftWorld, T_leftWorld, g_leftWorld,...
    R_wleft, T_wleft, g_wleft,...
    R_rightWorld, T_rightWorld, g_rightWorld,...
    R_wright, T_wright, g_wright] = unwrap_stereo_params (stereo_pair);

refIndex = refraction_params.ref_index;
gelHeight = refraction_params.gel_height;
n = refraction_params.gel_normal; %%surface normal
%refIndex = 1;
nPoints =  size (pix_right,2);
%gelHeight = -43;  %% this is with the acrylic sheet on the top IS THIS CORRECT

%pix_left = [xL yL]';
%LeftI = imread('images/L_000000000.ppm', 'ppm');
%LeftI = double(LeftI)/256 ;
%figure(100);
%imshow(LeftI)
%pix_left = ginput(nPoints)';
%pix_left = cornerfinder(pix_left, LeftI);

%pix_right = [xR yR]';
%figure;
%RightI = imread('R_000000000.ppm', 'ppm');
%RightI = double(RightI)/256 ;
%figure(200);
%imshow(RightI)
%pix_right= ginput(nPoints)'
%pix_right = cornerfinder(pix_right, RightI);

% %In an image pix_left/right= [colNum ; rowNum]

%close all %%close all the figures

 
% find c,p, theta, alpha in the world ref frame
%  camCenter_left = R_wleft * [0;0; 0] + T_wleft ;
%  camCenter_right = R_wright * [0;0; 0] + T_wright ;

camCenter_left = stereo_pair.T_left2world;
camCenter_right = stereo_pair.T_right2world;

%pixel width for our cameras in mm
%cellWidth = 4.65*1e-3;
cellWidth = stereo_pair.left.pixel_width;

% focal length in mm
% focal_left = mean(fc_left) ;
% focal_right = mean(fc_right);

focal_left = stereo_pair.left.fc;   %fc_left;
focal_right = stereo_pair.right.fc; %fc_right;

%--- Normalize the image projection according to the intrinsic parameters
%of the left and right cameras
% xn_left = normalize(pix_left,[focal_left; focal_left], cc_left,kc_left,alpha_c_left);
% xn_right = normalize(pix_right,[focal_right; focal_right], cc_right,kc_right,alpha_c_right);

xn_left = normalize(pix_left,focal_left, cc_left,kc_left,alpha_c_left);
xn_right = normalize(pix_right,focal_right, cc_right,kc_right,alpha_c_right);


%%finding theta and alpha angles for both the right and left cameras
%q_left = R_wleft * [xn_left;ones(size(xn_left,2))] * focal_left*cellWidth + repmat(T_wleft, 1,nPoints);
q_left = R_wleft * [xn_left;ones(1,size(xn_left,2))] * mean(focal_left)*cellWidth + repmat(T_wleft, 1,nPoints);
%q_right = R_wright * [xn_right;ones(size(xn_right,1))] * focal_right*cellWidth + repmat(T_wright, 1,nPoints);
q_right = R_wright * [xn_right;ones(1,size(xn_right,2))] * mean(focal_right)*cellWidth + repmat(T_wright, 1,nPoints);

%%% MAKE THIS INTO A FUNCTION
P_RefAccounted = zeros(3,nPoints);



if (refIndex == 1 || gelHeight == 0)
% If there is no refraction, simply use the triangulation script
     [XL, XR] = stereo_triangulation(pix_left, pix_right, stereo_pair.om, stereo_pair.T,...
         stereo_pair.left.fc, stereo_pair.left.cc, stereo_pair.left.kc, stereo_pair.left.alpha_c,...
         stereo_pair.right.fc, stereo_pair.right.cc, stereo_pair.right.kc,...
         stereo_pair.right.alpha_c);
     %Point_RefNotAccounted = R_wleft * XL + repmat(T_wleft,1,nPoints);
     Point_RefNotAccounted  = R_wright * XR + repmat(T_wright,1,nPoints);

     P_RefAccounted = Point_RefNotAccounted;
else

for i=1:nPoints
    q = q_left(:,i);
    camCenter = camCenter_left;
    k = (gelHeight-camCenter(3)) / (q(3)-camCenter(3));
    p_left = [(q(1)-camCenter(1))*k + camCenter(1);
                   (q(2)-camCenter(2))*k + camCenter(2);
                   gelHeight];
    theta_air_left = acos(abs(dot(q - p_left , n)/norm(q-p_left)));
    theta_gel_left = asin(sin(theta_air_left)  /  refIndex);

    q = q_right(:,i);
    camCenter = camCenter_right;
    k = (gelHeight-camCenter(3)) / (q(3)-camCenter(3));
    p_right = [(q(1)-camCenter(1))*k + camCenter(1);
                      (q(2)-camCenter(2))*k + camCenter(2);
                       gelHeight];
    theta_air_right = acos(abs(dot(q - p_right , n)/norm(q-p_right)));
    theta_gel_right = asin(sin(theta_air_right) /  refIndex);
    
    %%finding the 3D point

    %i_left = ray in air wrt left cam
    i_left = q_left(:,i) - camCenter_left;
    i_left = i_left/norm(i_left);
    %k_left is the direction perpendicular to surface normal,n in the plane containing the surface normal, incident ray and refracted ray
    k_left = i_left - dot(i_left, n) * n;
    k_left = k_left /norm(k_left);
    %gelHeight
    %%same things for the right camera
    i_right = q_right(:,i) - camCenter_right;
    i_right = i_right/norm(i_right);
    k_right = i_right - dot(i_right, n) * n;
    k_right = k_right/norm(k_right);
    
    %%setting up the linear system 
    %% p_left + S(1)*(s(theta_airleft)*k_left +cos(theta_gelright)*n)
    %% = p_right + S(2)*(sin(theta_gel_right)*k_right + cos(theta_gel_right)*n)
    A = [ (sin(theta_gel_left)*k_left + cos(theta_gel_left)*n)  -(sin(theta_gel_right)*k_right + cos(theta_gel_right)*n) ];
    b = [p_right - p_left];
    S = A \ b; %pinv(A)*b

    P = p_left + S(1)*(sin(theta_gel_left)*k_left + cos(theta_gel_left)*n);
    P1 = p_right + S(2)*(sin(theta_gel_right)*k_right + cos(theta_gel_right)*n);
    P_RefAccounted(:,i) = (P + P1 )/2;
end
end