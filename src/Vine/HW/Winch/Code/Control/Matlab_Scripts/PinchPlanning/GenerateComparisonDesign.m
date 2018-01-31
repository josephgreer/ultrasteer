clear; clc; close all;

load 'Maps/map'
load 'Maps/nodes'

map = map/8;
nodes = nodes/8;

rng(1);


addpath(genpath('Geom2d'));

figure;

handles.robot = [];


daspect([1 1 1]);
grid on;
hold on;
box on;
hold on;

useMidPointNodes = true;

DrawMap(map);

nNodes = size(nodes,1);
nodeTypes = zeros(nNodes,2);
wallEndPoints = [linspace(1,size(map,1),size(map,1)).'  map(:,1:2) ones(size(map,1),1);...
    linspace(1,size(map,1),size(map,1)).' map(:,3:4) 2*ones(size(map,1),1)];
for i=1:nNodes
    deltas = repmat(nodes(i,:),size(wallEndPoints,1),1)-wallEndPoints(:,2:3);
    dists = sqrt(sum(deltas.^2,2));
    [minDist, minIdx] = min(dists);
    if(minDist < 1e-3)
        nodeTypes(i,:) = [1 -1];
    else
        dists = DistanceToLines(map(:,1:2), map(:,3:4), nodes(i,:));
        [minDist, minIdx] = min(dists);
        
        lineIdx = FindLineSegmentIndex(map(:,1:2), map(:,3:4), nodes(i,:));
        if(lineIdx > 0)
            nodeTypes(i,:) = [2 lineIdx];
        else
            nodeTypes(i,:) = [3 -1];
        end
    end
end

for i=1:3
    currIs = nodeTypes(:,1) == i;
    if(useMidPointNodes || i ~= 2) 
        scatter(nodes(currIs,1),nodes(currIs,2), 'LineWidth',2);
    end
end

[x y] = ginput();

xs = [x y];
deltas = xs(2:end,:)-xs(1:end-1,:);

lengths = sqrt(sum(deltas.^2,2));

unitVectors = deltas./repmat(lengths,1,2);

unitVectors = [1 0; unitVectors];
designThetas = angleDiffSigns([unitVectors(1:end-1,:) zeros(size(unitVectors,1)-1,1)],...
    [unitVectors(2:end,:), zeros(size(unitVectors,1)-1,1)]);

designLs = [0; cumsum(lengths)];

save('Maps/NominalDesign2','designLs','designThetas');

