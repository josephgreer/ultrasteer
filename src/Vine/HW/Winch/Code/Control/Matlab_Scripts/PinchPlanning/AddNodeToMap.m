clear; clc; close all;

figure;
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

[x y] = ginput();

pos = [x y];

nodes = vertcat(nodes,pos);

scatter(nodes(:,1),nodes(:,2),'Linewidth',2);
k = waitforbuttonpress;

save('Maps/mapForest', 'map');
save('Maps/nodesForest', 'nodes');




