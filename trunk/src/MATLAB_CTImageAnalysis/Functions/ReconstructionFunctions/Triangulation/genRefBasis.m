function [ k ] = genRefBasis( ray, normal )
%GENREFBASIS Used to generate a basis for considering the incident ray (use
%k and the normal)

%i = incident ray (on gel)
i = ray / norm(ray) * -1;

%k_left is the direction perpendicular to surface normal,n in the plane
%containing the surface normal, incident ray and refracted ray
k = i - dot(i, normal) * normal;
%k = i - dot(normal,i) * normal;
k = k /norm(k);