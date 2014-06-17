function [ theta_air, theta_gel ] = refractionAngles( ray_air, refIndex, normal )
%REFRACTIONANGLE 
% Takes a ray that is parallel to the ray from the surface to the camera
% and the refractive index to return angle that the ray makes to the normal
% as well as the angle that the refracted ray will make with the normal
% Note here that ray_air is incident and ray_gel is the refracted ray


% Use the dot product to determine the angle between the projected
% ray and the surface normal.
theta_air = acos(abs(dot(ray_air , normal))/norm(ray_air));

%Using Snell's law, determine the angle of the ray in the tissue
%when it is refracted
theta_gel = asin(sin(theta_air)  /  refIndex);
