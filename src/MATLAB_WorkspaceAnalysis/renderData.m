function [ data ] = renderData( data )
%RENDERDATA Create figures to show meshes and reachability results

%% Render the meshes
% Initialize
figure(1); clf; hold on;

% Render the liver as a patch object
patch(data.mesh.lv, ...
            'FaceColor',       [0.8 0.2 0.2], ...
            'EdgeColor',       'none',        ...
            'FaceLighting',    'gouraud',     ...
            'FaceAlpha',       0.5,           ...
            'AmbientStrength', 0.5);

% Render the hepatic veins as a patch object
patch(data.mesh.obs(1), ...
            'FaceColor',      [0.2 0.2 0.9], ...
            'EdgeColor',       'none',        ...
            'FaceLighting',    'gouraud',     ...
            'FaceAlpha',       0.25,           ...
            'AmbientStrength', 0.5);

% Render the portal veins as a patch object
patch(data.mesh.obs(2), ...
            'FaceColor',      [0.2 0.9 0.2], ...
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


%% Render the target points
% Initialize
figure(2); clf; hold on;

% Render the data points
isTarget = logical(data.fineVol.V+1);
tX = data.fineVol.coords.X;
tX = tX(isTarget);
tY = data.fineVol.coords.Y;
tY = tY(isTarget);
tZ = data.fineVol.coords.Z;
tZ = tZ(isTarget);
plot3(tX, tY, tZ,'k.','LineWidth',0.2);

% Fix the axes scaling, and set a nice view angle
axis('image');
view([145 25]);

% Format the plot
grid on;
set(gcf,'color','w');

fprintf( 'Done in %.2f seconds...\n',toc );

%% Render the reachable points
% Initialize
figure(3); clf; hold on;

% Render the reachable points
isReachable = find(data.fineVol.V(:) > 0);
tX = data.fineVol.coords.X;
tX = tX(isReachable);
tY = data.fineVol.coords.Y;
tY = tY(isReachable);
tZ = data.fineVol.coords.Z;
tZ = tZ(isReachable);
plot3(tX, tY, tZ,'r.','LineWidth',0.2);

% Fix the axes scaling, and set a nice view angle
axis('image');
view([145 25]);

% Draw the insertion sheath
sth = data.entry.sheath.coords;
plot3(sth(1,:),sth(2,:),sth(3,:),'k','LineWidth',10);

% Format the plot
grid on;
set(gcf,'color','w');


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
contourslice(   data.fineVol.coords.X, ...
                data.fineVol.coords.Y, ...
                data.fineVol.coords.Z, ...
                data.fineVol.V, ...
                Sx, Sy, Sz, ...
                cvals   );
            
% Format the plot
grid on; 
axis equal;
set(gcf,'color','w');
colorbar;


%% Print results
PercentReachable = nnz(data.fineVol.V > 0) / nnz(data.fineVol.V >= 0)*100; 
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

end

