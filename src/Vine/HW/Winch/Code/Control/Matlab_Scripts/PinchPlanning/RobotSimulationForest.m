clear; clc; close all;

addpath(genpath('Geom2d'));

rng(4);

imName = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/ObstaclePlanning/Data/VineRobotForest/Perspective_Corrected.jpg';

initTheta = deg2rad(0);%deg2rad(348.1292);%deg2rad(unifrnd(0,360));
p0 = [0; 401];
x = [p0;p0;p0];
y = [0;0;0;1;0];
xs = [x(3) x(4); x(5) x(6)];

% thetas = [ zeros(0,2);%[131 -30; 301 45]; %101 30; 151 -30; 201 30; 251 -30];
% thetas = [         0  -19.2993;
%                   129.1126  -39.6000;
%                   259.0007   50;
%                   360.0118         0];
              
 thetas =[                 0  -30.0836
                          185.9399  -26.0724;
                          250.9479   28.0780;
                          280.6292  -55.4663;
                          396.7064   19.0355;
                          508.1181         0      ];     
                      
% thetas = [          0  -16.0446;
%                   149.8359  -34.0947;
%                   185.5994  -38.1058;
%                   423.7885   21.8997;
%                   515.8072         0
%   ];


% thetas = zeros(0,2);
thetas(:,2) = deg2rad(thetas(:,2));

figure;
imshow(imresize(imread(imName),[401,301]));
grid on;
hold on;

handles.robot = [];

xlim([0 301]);
ylim([0 401]);
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

[map,nodes] = ShrinkMap(map, nodes, 0.318*3);
DrawMap(map);
scatter(nodes(:,1),nodes(:,2),'LineWidth',1);

handles = DrawRobotXs(xs,-1,handles);
% frame = getframe(gcf);
% writeVideo(v,frame);

wallIndex = -1;

ls = [];
len = 0;

dls = thetas(2:end,1)-thetas(1:end-1,1);
thetas = thetas(1:end-1,:);
for i=1:length(dls)
    for j=1:10
        [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dls(i)/10, map, thetas, len, wallIndex, -1, xs);
        len = RobotLength(xs)
        handles = DrawRobotXs(xs,-1,handles);
        pause(0.1);
        
%         frame = getframe(gcf);
%         writeVideo(v,frame);
    end
    thetas(i+1,1) = RobotLength(xs);
end
    

% close(v);