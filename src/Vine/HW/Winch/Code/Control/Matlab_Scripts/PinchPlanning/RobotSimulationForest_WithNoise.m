clear; clc; close all;

addpath(genpath('Geom2d'));

rng(4);

%imName = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/ObstaclePlanning/Data/VineRobotForest/Perspective_Corrected.jpg';

scalar = 1/10*2.54;
initTheta = deg2rad(0);%deg2rad(348.1292);%deg2rad(unifrnd(0,360));
p0 = [0; 401]*scalar;
x = [p0;p0;p0];
y = [0;0;0;1;0];
xs = [x(3) x(4); x(5) x(6)];

% thetas = [ zeros(0,2);%[131 -30; 301 45]; %101 30; 151 -30; 201 30; 251 -30];
% thetas = [         0  -19.2993;
%                   129.1126  -39.6000;
%                   259.0007   50;
%                   360.0118         0];
%               
%  thetas =[                 0  -30.0836
%                           185.9399  -26.0724;
%                           250.9479   28.0780;
%                           280.6292  -55.4663;
%                           396.7064   19.0355;
%                           508.1181         0      ];        
                      
thetas = [          0  -16.0446;
                  149.8359  -34.0947;
                  185.5994  -38.1058;
                  423.7885   21.8997;
                  515.8072         0
  ];
 
thetas(:,1) = thetas(:,1)*scalar;                  


% thetas = zeros(0,2);
thetas(:,2) = deg2rad(thetas(:,2));

h = figure;
set(h, 'Position', [0 0 300 400]);
% imshow(imresize(imread(imName),[401,301]));
grid on;
hold on;

handles.robot = [];

xlim([0 301]*scalar);
ylim([0 401]*scalar);
daspect([1 1 1]);
%% 
grid on;
hold on;
box on;

% v = VideoWriter('Run3.avi');
% v.FrameRate = 10;
% open(v);

% load Maps/map1
load Maps/mapForest
load Maps/nodesForest
% map = GenerateForestMap(12);
% map = map*1.75;
map = map*scalar;
nodes = nodes*scalar;

% [map,nodes] = ShrinkMap(map, nodes, 0.318*3);
DrawMap(map);
% map = zeros(0,4);
% map = zeros(0,4);
% scatter(nodes(:,1),nodes(:,2),'LineWidth',1);

handles = DrawRobotXs(xs,-1,handles);
% frame = getframe(gcf);
% writeVideo(v,frame);

wallIndex = -1;

ls = [];
len = 0;

thetas =          [0   -0.2800;
                   38.0583   -0.9361;
                   47.1422   -0.8198;
                   77.1447   -0.0177;
                   107.6423    0.3822;
                   131.0150         0];

npoints = 100;
dtheta = 10;
thetaPerturb = unifrnd(-deg2rad(dtheta),deg2rad(dtheta), [size(thetas,1) 5*npoints]);
thetaPerturb(1,:)  =0;
% thetaPerturb(2,:) = 0;
% thetaPerturb(:,1) = 0;
backup_thetas = thetas;

saved_xs = {};
ii = 0;
while length(saved_xs) < npoints
    ii = ii+1;
    try
        scalar = 1/10*2.54;
        p0 = [0; 401]*scalar;
        x = [p0;p0;p0];
        y = [0;0;0;1;0];
        xs = [x(3) x(4); x(5) x(6)];
        wallIndex = -1;
        
        ls = [];
        len = 0;
        thetas = backup_thetas;
        thetas(:,2) = thetas(:,2)+thetaPerturb(:,ii);
        dls = thetas(2:end,1)-thetas(1:end-1,1);
        thetas = thetas(1:end-1,:);
        for i=1:length(dls)
            for j=1:10
                [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dls(i)/10, map, thetas, len, wallIndex, -1, xs);
                len = RobotLength(xs)
                handles = DrawRobotXs(xs,-1,handles);
                %         pause(0.1);
                
                %         frame = getframe(gcf);
                %         writeVideo(v,frame);
            end
            thetas(i+1,1) = RobotLength(xs);
        end
        if(abs(RobotLength(xs)-backup_thetas(end,1))>0.1)
            continue;
        end
        saved_xs = vertcat(saved_xs, {xs});
    catch
        blah  =0;
    end
end

set(gca, 'FontSize', 12, 'FontName', 'Times New Roman');
box on;

%%
delete(handles.robot);
hold on;
pt_density = 2;

heatmap_points = [];

for ii = 1:npoints
    xs = saved_xs{ii};
    
    for j=2:size(xs,1)-1
       delta = xs(j+1,:)-xs(j,:);
       length = norm(delta);
       np = ceil(length*pt_density);
       
       delta = repmat(linspace(0,np,np+1).', 1, 2).*repmat(delta/np,np+1,1);
       pts = repmat(xs(j,:),np+1,1)+delta;
       heatmap_points = vertcat(heatmap_points, pts);
       scatter(pts(:,1),pts(:,2),20,'MarkerFaceColor','b','MarkerEdgeColor','b','MarkerFaceAlpha',0.01, 'MarkerEdgeAlpha',0.01);
    end
end

%%
points = heatmap_points;
grd = 32;   %refinement of map
minvals = min(points);
maxvals = max(points);
rangevals = maxvals - minvals;
xidx = 1 + round((points(:,1) - minvals(1)) ./ rangevals(1) * (grd-1));
yidx = 1 + round((points(:,2) - minvals(2)) ./ rangevals(2) * (grd-1));
xidx(isnan(xidx)) = [];
yidx(isnan(yidx)) = [];

density = accumarray([yidx, xidx], 1, [grd,grd]);  %note y is rows, x is cols
figure;
imagesc(density, 'xdata', [minvals(1), maxvals(1)], 'ydata', [minvals(2), maxvals(2)]);
    

% close(v);