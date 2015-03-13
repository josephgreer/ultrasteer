function [ data ] = createVolume( data, volName, in )
%SAMPLEVOLUME Create a 3D volume at spacing specificed in DEL, and save
% under VOLNAME in the data struct.


%% Sample a grid of points

% Extract the liver model
fv = data.mesh.lv; 

% Measure extents of liver model
vol.min = floor(min(fv.vertices))'; 
vol.max = ceil(max(fv.vertices))';
    
% Define spacing in each direction
volSpecs = getfield(in,volName);
vol.del  = [volSpecs.del volSpecs.del volSpecs.del]';

% Create the grid of points
for i = 1:3
    if isempty(volSpecs.slc{i})
        vxyz{i} = vol.min(i): vol.del(i): vol.max(i);
    else
        vxyz{i} = volSpecs.slc{i};
    end
end
[X,Y,Z] = meshgrid(vxyz{1},vxyz{2},vxyz{3});
vol.coords.X = X;
vol.coords.Y = Y;
vol.coords.Z = Z;
vol.coords.XYZ = [X(:) Y(:) Z(:)]';
vol.coords.mXYZ = repmat([X(:) Y(:) Z(:)]',...
                            [1,1,100]);

% Record the grid dimensions and total number of points
vol.dim = size(X);
vol.Npts = length(vol.coords.XYZ);


%% Identify points inside obstacles and outside liver

% Create the step value matrix for the volume
vol.V = zeros(vol.dim);

% Exclude points outside the liver mesh
inlv = inpolyhedron(data.mesh.lv,vol.coords.XYZ');
vol.V(~inlv) = -1;
vol.V = reshape(vol.V,vol.dim);

% Exclude points inside the obstacles
if( any(strcmp('obs',fieldnames(data.mesh))) )
    for j = 1:length(data.mesh.obs)
        inobs(:,j) = inpolyhedron(data.mesh.obs(j),vol.coords.XYZ');
    end
    ineither = logical(sum(inobs,2));
    vol.V(ineither) = -1;
    vol.V = reshape(vol.V,vol.dim);
end

% Create the radius value matrix for the volume
vol.R = zeros(vol.dim);


%% Save the volume into the data struct
data = setfield(data, volName, vol);



end

