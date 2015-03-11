% Troy Adebar Stanford University 2/24/2015
%
% WORKSPACEANALYSIS Loads mesh data for a segmented liver, and analyzes the
% reachable workspace for steerable needles with different geometries
%
%
%% Initialize
% clear all; 
clc; 
tic; 
clear data inobs;

%% Define some initialization values/constants
in.Rmin          = 10;             % [mm] the minimum achievable curvature
in.darcpts       = 8;              % [mm] the linear spacing between arc pts

in.fineVol.del   = 1;              % [mm] the voxel dimension for fine vol
in.fineVol.slc   = {[],[],0};      % [mm] the position of slices for 2D anal

in.coarseVol.del = 30;             % [mm] the voxel dimension for coarse vol
in.coarseVol.slc = {[],[],[]};     % [mm] the position of slices for 2D anal


in.p             = [97 -80 0]';    % introducer point
in.v             = [0 1 0]';       % introducer vector (unit vector)

t                = [];             % vector of runtimes in seconds

%% Intialize the data struct
tic;
fprintf( 'Initializing data struct...\n'); 
data = initialize( in );
t = [t toc];
fprintf( 'Done in %.2f seconds...\n\n',t(end) );


%% Define fine and coarse volumes
tic;
fprintf( 'Defining fine volume...\n'); tic;
data = createVolume(data,'fineVol',in);
t = [t toc];
fprintf( 'Done in %.2f seconds...\n\n',t(end) );

tic;
fprintf( 'Defining coarse volume...\n'); 
data = createVolume(data,'coarseVol',in);
t = [t toc];
fprintf( 'Done in %.2f seconds...\n\n',t(end) );


%% Create boundaries at mesh points to check entering/leaving liver 
tic;
fprintf( 'Creating boundaries...\n'); 
data = createBoundaries(data,'coarseVol');
t = [t toc];
fprintf( 'Done in %.2f seconds...\n\n',t(end) );


%% Find reachable points on first iteration
tic;
fprintf( 'Checking single-arc paths...\n'); tic;
data.itr = 1;
data = checkReachable(data);
t = [t toc];
fprintf( 'Done in %.2f seconds...\n\n',t(end) );


%% Find reachable points on second iteration
tic;
fprintf( 'Checking double-arc paths...\n'); 
data = checkReachable(data); 
t = [t toc];
fprintf( 'Done in %.2f seconds...\n\n',t(end) );


%% Visualize the results
tic;
fprintf( 'Visualizing results...\n');
data.runtimes = t;
data = renderData(data);
t = [t toc];
fprintf( 'Done in %.2f seconds...\n\n',t(end) );


%% Save the results
tic;
fprintf( 'Saving results...\n');
save('FirstResults','data');
t = [t toc];
fprintf( 'Done in %.2f seconds...\n\n',t(end) );

% eof
