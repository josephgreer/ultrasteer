% Troy Adebar Stanford University 2/24/2015
%
% WORKSPACEANALYSIS Loads mesh data for a segmented liver, and analyzes the
% reachable workspace for steerable needles with different geometries
%
%
%% Initialize
% clear all; 
clc; tic;
data.count = 1;

%% Define the minimum achievable curvature
data.Rmin = 50;     % mm


%% Import an STL mesh, returning a PATCH-compatible face-vertex structure
fv = stlread('stl/liver.stl');
data.mesh.lv = fv;


%% Define a bounding cylinder around the hepatic vessels
ptl = stlread('stl/portalveins.stl');
hep = stlread('stl/hepaticveins.stl');
data.mesh.obs(1) = ptl;
data.mesh.obs(2) = hep;


%% Define the insertion point and orientation
% Define insertion point, vector, and length
entry.p = [97 -80 0]';
entry.v = [-1 1 0]'./sqrt(2); % Must be a unit vector
l = 25;
entry.sheath.coords = [entry.p-l.*entry.v entry.p];
data.entry = entry;


%% Render the meshes
% Initialize
figure(1); clf; hold on;

% Render the liver as a patch object
patch(fv,   'FaceColor',       [0.8 0.2 0.2], ...
            'EdgeColor',       'none',        ...
            'FaceLighting',    'gouraud',     ...
            'FaceAlpha',       0.5,           ...
            'AmbientStrength', 0.5);

% Render the hepatic veins as a patch object
patch(hep,  'FaceColor',      [0.2 0.2 0.9], ...
            'EdgeColor',       'none',        ...
            'FaceLighting',    'gouraud',     ...
            'FaceAlpha',       0.25,           ...
            'AmbientStrength', 0.5);

% Render the portal veins as a patch object
patch(ptl,  'FaceColor',      [0.2 0.9 0.2], ...
            'EdgeColor',       'none',        ...
            'FaceLighting',    'gouraud',     ...
            'FaceAlpha',       0.25,           ...
            'AmbientStrength', 0.5);

% Fix the axes scaling, and set a nice view angle
axis('image');

% Add a camera light, and tone down the specular highlighting
view([145 25]);
camlight('headlight');
material('dull');

% Draw the insertion sheath
sth = data.entry.sheath.coords;
plot3(sth(1,:),sth(2,:),sth(3,:),'k','LineWidth',10);
plot3(sth(1,2),sth(2,2),sth(3,2),'k*','LineWidth',10);

% Format the plot
grid on;
set(gcf,'color','w');


%% Define sampling points
% Measure extents of model
pts.min = floor(min(fv.vertices)); 
pts.max = ceil(max(fv.vertices));
    
% Define spacing in each direction    
pts.del  = [15 15 15];

% Create the grid of points
for i = 1:3
    vxyz{i} = pts.min(i):pts.del(i):pts.max(i);
end
[X,Y,Z] = meshgrid(vxyz{1},vxyz{2},vxyz{3});
pts.coords.X = X;
pts.coords.Y = Y;
pts.coords.Z = Z;
pts.coords.XYZ = [X(:) Y(:) Z(:)];

% Record the grid dimensions and total number of points
pts.dim = size(X);
pts.Npts = length(pts.coords.XYZ);

% Save the points struct
data.pts = pts;

% Save the "reachable" value struct
data.V = zeros(pts.dim);

%% Identify non-target points
% Exclude points outside the mesh
inlv = inpolyhedron(data.mesh.lv,data.pts.coords.XYZ);
data.V(~inlv) = -1;
data.V = reshape(data.V,data.pts.dim);

% Exclude points inside the obstacles
if( any(strcmp('obs',fieldnames(data.mesh))) )
    for j = 1:length(data.mesh.obs)
        inobs(:,j) = inpolyhedron(data.mesh.obs(j),data.pts.coords.XYZ);
    end
    ineither = logical(sum(inobs,2));
    data.V(ineither) = -1;
    data.V = reshape(data.V,data.pts.dim);
end

%% Render the target points
% Initialize
figure(2); clf; hold on;

% Render the data points
isTarget = logical(data.V+1);
tX = data.pts.coords.X;
tX = tX(isTarget);
tY = data.pts.coords.Y;
tY = tY(isTarget);
tZ = data.pts.coords.Z;
tZ = tZ(isTarget);
plot3(tX, tY, tZ,'k.','LineWidth',0.2);

% Fix the axes scaling, and set a nice view angle
axis('image');
view([145 25]);

% Format the plot
grid on;
set(gcf,'color','w');


%% Find reachable points on first iteration
data.itr = 1;
data = checkReachable(data);  


%% Find reachable points on second iteration
data = checkReachable(data);  


% %% Render the reachable points
% % Initialize
% figure(3); clf; hold on;
% 
% % Render the original data points
% plot3(pts.coords(:,1), pts.coords(:,2), pts.coords(:,3), ...
%     'ko', 'LineWidth',0.2);
% 
% % Exclude the unreachable data points
% pts.coords((repmat(~logical(data.reachable),1,3))) = [];
% pts.coords = reshape(pts.coords,[],3);
% 
% % Render the reachable data points
% plot3(pts.coords(:,1), pts.coords(:,2), pts.coords(:,3), ...
%     'ro','MarkerFaceColor','r','LineWidth',0.2);
% 
% % Fix the axes scaling, and set a nice view angle
% axis('image');
% view([145 25]);
% 
% % Draw the insertion sheath
% sth = data.entry.sheath.coords;
% plot3(sth(1,:),sth(2,:),sth(3,:),'k','LineWidth',10);
% 
% % Format the plot
% grid on;
% set(gcf,'color','w');


%% Visualize the reachable workspace
% Initialize
figure(4); clf; hold on;

% Define plane locations in each axis
Sx = [];
Sy = [];
Sz = [10 50];

% Define color contours
cvals = [0 1 2];

% Draw contours
contourslice(   data.pts.coords.X, ...
                data.pts.coords.Y, ...
                data.pts.coords.Z, ...
                data.V, ...
                Sx, Sy, Sz, ...
                cvals   );
            
% Format the plot
grid on; 
axis equal;
set(gcf,'color','w');
colorbar;


%% Visualize the reachable workspace
% Initialize
figure(5); clf; hold on;

% Define plane locations in each axis
Sx = [];
Sy = [];
Sz = [10 50];

% Define color contours
cvals = [0 1 2];

% Draw contours
contourslice(   data.pts.coords.X, ...
                data.pts.coords.Y, ...
                data.pts.coords.Z, ...
                data.V, ...
                Sx, Sy, Sz, ...
                cvals   );
            
% Format the plot
grid on; 
axis equal;
set(gcf,'color','w');
colorbar;


%% Print results
PercentReachable = nnz(data.V > 0) / nnz(data.V >= 0)*100; 
fprintf('\n\n')

fprintf('--------------------------------------------------------------\n')
fprintf('--------------------------------------------------------------\n')

fprintf('\n')
fprintf( 'Total runtime is %.2f minutes...\n',...
    toc/60 );
fprintf( 'With Rmin = %.2f, %.2f%% of the workspace is reachable...\n',...
    data.Rmin, PercentReachable );
fprintf('\n')

fprintf('--------------------------------------------------------------\n')
fprintf('--------------------------------------------------------------\n')

%% Save the results
save('FirstResults','data');

% eof
