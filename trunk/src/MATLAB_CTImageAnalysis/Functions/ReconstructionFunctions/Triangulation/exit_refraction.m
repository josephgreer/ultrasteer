function [true_epoint, surface_projection] =  exit_refraction (point, T_wcam, normal, gelHeight, refIndex, ...
    R_camWorld, T_camWorld,fc_cam,cc_cam,kc_cam, alpha_c_cam, isGraphicsOn )
% This function compensates for exit refraction.
% Accepts the location of a point at the bottom of the gel and returns how
% it projects back to a camera.
% Author: Tom Wedlick

% Load the solution to the refraction equations
%load refractionSolution.mat


%% initialize the variables
true_epoint =[];
surface_projection = [];

%% Define the points in the system

% Define a basis for the plane that the point will be in
% one basis vector is the surface normal n

% consider the vector connecting the point of consideration with the camera
w_world = T_wcam  - point;

% project that vector onto a vector that is orthoganal to the normal vector
% this will form the second vector of that basis
%k = w_world - n*dot (w_world, n) /norm (n);k = k /norm(k)
k = genRefBasis(w_world,normal);

% determine the projections of the point of interest (w) in the constructed
% basis (w and n)
Wk = dot (w_world,k);
Wn = dot (w_world,normal);

% consider the point on the gel surface that the incident ray from the
% point of interest intersects
An = dot(T_wcam,normal) - gelHeight  ;

% 
% if (Wk < 0)
%     k = k * - 1;
%     Wk = Wk * -1
% end

%% Display the basis 
%plot the basis

if (isGraphicsOn)
    figure(2)
    hold on
    drawVector(point, point+k*10,'-b')
    hold on
    drawVector(point, point+normal*10,'-g')

    %plot the ray connecting the point of interest to the camera
    hold on
    drawVector(point,w_world + point,':g*')
    axis('equal')
end

%Form an initial guess on the incident angle
%initialGuess =  (0 +  atan2 (abs(Wk), abs(Wn))    )/2

%% Solve the geometry for the projection point on the tissue surface

%refractionGeometry; %<-- faster than symbolic solve
RefractionSolution; % <-- faster solution when it uses fsolve

%% Determine the point on the surface and project that onto the camera


%Now that the distance Ak has been determined, detrmine the point
%on the gel surface
%dist = Wk -solution(index(i));
dist = Wk - Ak;

Surface_point = point + k * dist + (Wn - An)*normal;

%Project the point on the gel back to the camera
elinePt = point2image (Surface_point , R_camWorld, T_camWorld,fc_cam,cc_cam,kc_cam, alpha_c_cam);

surface_projection = Surface_point';
true_epoint = elinePt';
