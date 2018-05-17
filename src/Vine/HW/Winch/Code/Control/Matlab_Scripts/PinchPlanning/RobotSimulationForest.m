clear; clc; close all;

addpath(genpath('Geom2d'));

rng(4);

imName = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/ObstaclePlanning/Data/VineRobotForest/Perspective_Corrected.jpg';

initTheta = deg2rad(-40);%deg2rad(348.1292);%deg2rad(unifrnd(0,360));
p0 = [0; 401];
x = [p0;p0;p0(1)+cos(initTheta);p0(2)+sin(initTheta)];
y = [0;0;0;1;0];
xs = [x(3) x(4); x(5) x(6)];

thetas = zeros(0,2);%[131 -30; 301 45]; %101 30; 151 -30; 201 30; 251 -30];

% thetas = zeros(0,2);
thetas(:,2) = deg2rad(thetas(:,2));

figure;
imshow(imresize(imread(imName),401/1820));
grid on;
hold on;

handles.robot = [];

xlim([0 301]);
ylim([0 401]);
daspect([1 1 1]);
grid on;
hold on;
box on;

% load Maps/map1
load Maps/mapForest
load Maps/nodesForest
% map = GenerateForestMap(12);
% map = map*1.75;
DrawMap(map);
scatter(nodes(:,1),nodes(:,2),'LineWidth',1);

handles = DrawRobotXs(xs,-1,handles);

wallIndex = -1;

ls = [];
len = 0;
dl = 10;
for i=1:100
    [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dl, map, thetas, len, wallIndex, -1, xs);
    len = RobotLength(xs)
    handles = DrawRobotXs(xs,-1,handles);
    pause(0.1);
end
    