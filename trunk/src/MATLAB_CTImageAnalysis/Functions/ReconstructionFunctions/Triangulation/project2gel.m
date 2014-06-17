function [x, p, ray] = project2gel(pix, R_w, T_w, g_w, cc, kc,...
    alpha_c ,gel_height, pixel_pitch, focal_length, graphics_on)
% Created by Tom Wedlick, Summer 2008, using code from Vinutha Kallem
% Projects the pixel coordinate onto the gel surface
% assumes that WorldFrame has been called

%input:
% pixels : The selected pixes
% R_w : The rotation matrix from the camera frame to the world frame
% T_w : The translation from the camera frame to the world frame
% cc, kc, alpha_c : camera parameters
% gel_height, the height of the gel above the origin in the world frame
% (z-direction), should be a negative value
% pixel pitch: pixel pitch, the  width between pixel centers (selected by vinutha)
% focal_length, in pixels
% graphics_on: whether the geometry should be displayed. 

%note that this function assumes the use of both the camera calibration
%toolbox and the epipolar geometry toolbox

%output:
% x: the imediate projection of the pixel in the camera frame (distance in z is
%       random)
% p: the point on the gel that the pixel projects to
% ray: the ray (in a coordinate system centered at p and parallel to the
% world reference frame), from the pixel to the point on the gel


%Define the gel surface normal
%n = [ 0;0;1];

%nPoints = size(pix,2);


%Define the camera center in the world coordinates
camCenter = R_w * [0;0; 0] + T_w ;

%--- Normalize the image projection according to the intrinsic parameters
%of the camera
% ~~ This converts pixel coordinates into normalized pixel coordinate, see
% the function normalize.m
%xn = normalize(pix,[focal_length; focal_length],cc,kc,alpha_c);
if (size(pix,1) < 2)
    pix = pix'
end
xn = normalize(pix,[focal_length; focal_length],cc,kc,alpha_c);

%% Convert the normalized pixel coordinates into real world coorfinates
% (they are still in the camera frames,...
% but now on the image plane), still only up to scale
x = [xn;ones(size(xn,2))] * focal_length* pixel_pitch;


% location to point in the world frame (location to x in the world frame)
q = R_w * x + T_w;%repmat(T_w, 1,nPoints);


thetas_air = [];
thetas_gel = [];

    
        % Use similar triangules to locate the point on the gel surface
        % that the ray leaves the gel surface towards the camer's center of
        % projection
        k = (gel_height-camCenter(3)) / (q(3)-camCenter(3)); %  = vert dist to gel top / vert dist to point
        
        % Define a vector from the world coordinate frame to the top of the
        % gel that is the intersection of the pixel ray and the gel surface
        p = [(q(1)-camCenter(1))*k + camCenter(1);
                       (q(2)-camCenter(2))*k + camCenter(2);
                       gel_height];
                   
                   
        % The ray from the camera to a porjected point on the gel
        ray = q - p;
       
        if (graphics_on)
            f_3Dwf('k');
            % need world to camera transformation
           f_3Dframe(g_w,'b',50,'R') % Plot the camera frame
           hold on
           f_3Dcamera(g_w,'b',10) % Plot the camera frame

           hold on
           f_3Dframe([eye(3,3),zeros(3,1);zeros(1,3),1],'r',50,'_{world}') %Plot the world frame origin

           hold on
           drawVector(g_w*[x;1], g_w*[0;0;0;1],'-x')

           %hold on
           %drawVector(q, [0,0,0],'--+g')

           %hold on
           %drawVector(p, [0,0,0],'-ob')

           hold on
           drawVector(ray+p,p+ [0;0;0],'-sr')

           %hold on
           %drawVector(ray,[0;0;0],'-sy')
 
        end

    %i_left = ray in air wrt left cam
    %i = q - camCenter;
    %i = i/norm(i);
    
    %dot (ray, i) / norm(ray) * -1
    
        