clear; clc; close all;

imName = '~/Desktop/AR0712SR.jpg';

imshow(imread(imName));

[x y] = ginput();


for i=1:length(x)
    distsX = abs(x-x(i));
    distsY = abs(y-y(i));
    
    nodes(distsX < 45) = x(i);
    y(distsY < 45) = y(i);
end

nodes = [x y];

scatter(nodes(:,1), nodes(:,2));

save('Maps/nodes', 'nodes');

%%
close all;

load('Maps/nodes');

imName = '~/Desktop/AR0712SR.jpg';

imshow(imread(imName));
hold on;
scatter(nodes(:,1), nodes(:,2));

%%
x = nodes(:,1);
y = nodes(:,2);
for i=1:length(x)
    distsX = abs(x-x(i));
    distsY = abs(y-y(i));
    
    nodes(distsX < 15) = x(i);
    nodes(distsY < 15) = y(i);
end

%%
clear; clc; close all;

load('Maps/nodes');
load('Maps/map');

imName = '~/Desktop/AR0712SR.jpg';

imshow(imread(imName));
hold on;
scatter(nodes(:,1), nodes(:,2));
DrawMap(map);

nx = nodes(:,1);
ny = nodes(:,2);

while(true)
    [x y] = ginput(2);
    
    deltas1 = [nx-x(1) ny-y(1)];
    deltas2 = [nx-x(2) ny-y(2)];
    
    deltas1 = sum(deltas1.^2,2);
    deltas2 = sum(deltas2.^2,2);
    
    [~,i1] = min(deltas1);
    [~,i2] = min(deltas2);
    
    map = vertcat(map, [nodes(i1,1) nodes(i1,2) nodes(i2,1) nodes(i2,2)]);
    DrawMap(map);
end
save('Maps/map','map');

%%
load('Maps/map1');
nodes = vertcat(map(:,1:2),map(:,3:4));
display(size(nodes,1));
nx = nodes(:,1);
ny = nodes(:,2);

display(length(nx))

badIdxs = [];
for i=1:size(nx)-1
    display(i)
    deltas = [nx(i+1:end)-nx(i) ny(i+1:end)-ny(i)];
    deltaNorms = sqrt(sum(deltas.^2,2));
    repeatIdxs = find(deltaNorms < 1e-3);
    badIdxs = union(badIdxs,repeatIdxs+i);
end
save('Maps/nodes1')

%%
load('Maps/nodes1');
load('Maps/map1');

scatter(nodes(:,1), nodes(:,2));
hold on;
DrawMap(map);

nx = nodes(:,1);
ny = nodes(:,2);

nodeIdx1 = [];

while(true)
    [x y] = ginput(1);
    
    deltas1 = [nx-x(1) ny-y(1)];
    
    deltas1 = sum(deltas1.^2,2);
    
    [~,i1] = min(deltas1);
    
    nodeIdx1 = vertcat(nodeIdx1, i1);
    scatter(nodes(i1,1),nodes(i1,2),'r');
end

%%
[x y] = ginput();
nodeIdx1 = vertcat(nodeIdx1,[x y]);
scatter(nodeIdx1(:,1),nodeIdx1(:,2));

%% 
[x y] = ginput();
nodes = vertcat(nodes,[x y]);
scatter(nodes(:,1),nodes(:,2));

%%
[x y] = ginput();
scatter(nodes(:,1),nodes(:,2));

newPoints = [];
for i=1:size(x,1)
    dists = DistanceToLines(map(:,1:2), map(:,3:4), [x(i) y(i)]);
    [~, minIdx] = min(dists);
    
    newPoint = ProjectPointOntoLine(map(minIdx,1:2),map(minIdx,3:4), [x(i) y(i)]);
    newPoints = vertcat(newPoints, newPoint);
end
scatter(newPoints(:,1),newPoints(:,2));