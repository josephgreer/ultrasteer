function [ data ] = createBoundaries( data, volName )
%CREATEBOUNDARIES Create a list of boundary points associated with
%voxels in the volume specified in volName.


% Find coordinates for all boundary meshes in the data struct
coords = [      data.mesh.lv.vertices'      ...
                data.mesh.obs(1).vertices'  ...
                data.mesh.obs(2).vertices'      ];
    
% Fine number of boundary points
N = length(coords);

% Extract the volume to associate points with
vol = getfield(data, volName);

% Associate all boundary points with voxels in vol
for i = 1:vol.Npts
    point = vol.coords.XYZ(:,i);
    
    ind = all( abs(coords-repmat(point,1,N)) < ...
    repmat(vol.del,1,N),1);

    boundaries.coords(i).XYZ = coords(:,ind);
    boundaries.coords(i).mXYZ = repmat(coords(:,ind),[1,1,data.Narcpts]);
    boundaries.N(i) = nnz(ind);
end

% Save the volume into the data struct
data.boundaries = boundaries;

end

