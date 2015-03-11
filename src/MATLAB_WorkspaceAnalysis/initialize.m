function [ data ] = initialize( in )
%INITIALIZE Create the data struct and intialize contants

%% Define some constants
data.Rmin       = in.Rmin;     
data.darcpts    = in.darcpts;     


%% Import STL meshes, returning PATCH-compatible face-vertex structures
fv = stlread('stl/liversparse.stl');
data.mesh.lv = fv;

ptl = stlread('stl/portalveinssparse.stl');
data.mesh.obs(1) = ptl;

hep = stlread('stl/hepaticveinssparse.stl');
data.mesh.obs(2) = hep;


%% Define the insertion sheath point and vector
entry.p = in.p;
entry.v = in.v; % Must be a unit vector
l = 25;
entry.sheath.coords = [entry.p-l.*entry.v entry.p];
data.entry = entry;


end

