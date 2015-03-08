function [ data ] = initialize( Rmin, Narcpts, p, v )
%INITIALIZE Create the data struct and intialize contants

%% Define some constants
data.Rmin       = Rmin;     
data.Narcpts    = Narcpts;     


%% Import STL meshes, returning PATCH-compatible face-vertex structures
fv = stlread('stl/liversparse.stl');
data.mesh.lv = fv;

ptl = stlread('stl/portalveinssparse.stl');
data.mesh.obs(1) = ptl;

hep = stlread('stl/hepaticveinssparse.stl');
data.mesh.obs(2) = hep;


%% Define the insertion sheath point and vector
entry.p = p;
entry.v = v; % Must be a unit vector
l = 25;
entry.sheath.coords = [entry.p-l.*entry.v entry.p];
data.entry = entry;


end

