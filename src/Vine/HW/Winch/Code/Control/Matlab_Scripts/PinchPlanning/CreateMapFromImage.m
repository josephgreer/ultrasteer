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