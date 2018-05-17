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

[x y] = ginput(1);

pos = [x y];

delts = repmat(pos,size(nodes,1),1)-nodes;
delts = sum(delts.^2,2);
[minVal,nodeIdx] = min(delts);

pos = nodes(nodeIdx,:);

delts1 = repmat(pos, size(map,1), 1)-map(:,1:2);
delts2 = repmat(pos, size(map,1), 1)-map(:,3:4);
delts1 = sum(delts1.^2,2);
delts2 = sum(delts2.^2,2);

[minVal1, idx1]  = min(delts1);
[minVal2, idx2]  = min(delts2);

assert(minVal1 < 1e-5);
assert(minVal2 < 1e-5);

newWall = [map(idx2,1:2) map(idx1,3:4)];

map(idx1,:) = newWall;
map(idx2,:) = [];

nodes(nodeIdx,:) = [];

DrawMap(map);
scatter(nodes(:,1),nodes(:,2));

save('Maps/mapForest', 'map');
save('Maps/nodesForest', 'nodes');




