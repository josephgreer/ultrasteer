% Troy Adebar Stanford University 2/24/2015
%
% WORKSPACEANALYSIS Loads mesh data for a segmented liver, and analyzes the
% reachable workspace for steerable needles with different geometries
%
%
%% Initialize
% clear all; 
clc; tic; 
clear data inobs;

%% Define some constants
Rmin       = 50;                % [mm] the minimum achievable curvature
Narcpts    = 25;                % the number of points check on each arc
fineDel    = 15;                % [mm] the voxel dimension for fine vol
coarseDel  = 25;                % [mm] the voxel dimension for coarse vol
p          = [97 -80 0]';       % introducer point
v          = [-1 1 0]'./sqrt(2);% introducer vector (unit vector)


%% Intialize the data struct
fprintf( 'Initializing data struct...\n'); tic;
data = initialize( Rmin, Narcpts, p, v );
fprintf( 'Done in %.2f seconds...\n\n',toc );


%% Define fine and coarse volumes
fprintf( 'Defining fine volume...\n'); tic;
data = createVolume(data,'fineVol',[fineDel fineDel fineDel]');
fprintf( 'Done in %.2f seconds...\n\n',toc );

fprintf( 'Defining coarse volume...\n'); tic;
data = createVolume(data,'coarseVol',[coarseDel coarseDel coarseDel]');
fprintf( 'Done in %.2f seconds...\n\n',toc );


%% Create boundaries at mesh points to check entering/leaving liver 
fprintf( 'Creating boundaries...\n'); tic;
data = createBoundaries(data,'coarseVol');
fprintf( 'Done in %.2f seconds...\n\n',toc );


%% Find reachable points on first iteration
fprintf( 'Checking single-arc paths...\n'); tic;
data.itr = 1;
data = checkReachable(data);
fprintf( 'Done in %.2f seconds...\n\n',toc );  


%% Find reachable points on second iteration
fprintf( 'Checking double-arc paths...\n'); tic;
data = checkReachable(data); 
fprintf( 'Done in %.2f seconds...\n\n',toc );   


%% Visualize the results
fprintf( 'Visualizing results...\n'); tic;
data = renderData(data);
fprintf( 'Done in %.2f seconds...\n\n',toc );


%% Save the results
fprintf( 'Saving results...\n'); tic;
save('FirstResults','data');
fprintf( 'Done in %.2f seconds...\n\n',toc );

% eof
