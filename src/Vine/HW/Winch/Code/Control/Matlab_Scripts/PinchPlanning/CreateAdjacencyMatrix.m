clear; clc; close all;

load 'Maps/map1'
load 'Maps/nodes1'


addpath(genpath('Geom2d'));

figure;
grid on;
hold on;

handles.robot = [];

xlim([-500 500]);
ylim([-500 500]);
daspect([1 1 1]);
grid on;
hold on;

DrawMap(map);

nNodes = size(nodes,1);
nAngles = 360;

% nodeType 1 = wall corner
%          2 = wall midpoint
%          3 = open space
% nodeTypes = [nodeType wallIndex] (note that if nodeType = 3, then
% wallIndex is irrelevant
nodeTypes = zeros(nNodes,2);
wallEndPoints = [linspace(1,size(map,1),size(map,1)).'  map(:,1:2) ones(size(map,1),1);...
    linspace(1,size(map,1),size(map,1)).' map(:,3:4) 2*ones(size(map,1),1)];
h = scatter(0,0);
for i=1:nNodes
    set(h,'XData',nodes(i,1),'YData',nodes(i,2));
    deltas = repmat(nodes(i,:),size(wallEndPoints,1),1)-wallEndPoints(:,2:3);
    dists = sqrt(sum(deltas.^2,2));
    [minDist, minIdx] = min(dists);
    if(minDist < 1e-3)
        nodeTypes(i,:) = [1 -1];
    else
        dists = DistanceToLines(map(:,1:2), map(:,3:4), nodes(i,:));
        [minDist, minIdx] = min(dists);
        if(minDist < 1e-3)
            nodeTypes(i,:) = [2 minIdx];
        else
            nodeTypes(i,:) = [3 -1];
        end
    end
end

% show the different types of nodes
for i=1:3
    currIs = nodeTypes(:,1) == i;
    scatter(nodes(currIs,1),nodes(currIs,2));
end

interiorNodes = find(nodeTypes(:,1) == 3);
nInteriorNodes = size(interiorNodes,1);

destinations = zeros(nNodes*nAngles,1);
thetas = linspace(0,2*pi,nAngles).';
for i=1:nNodes
    % first figure out which interior nodes we have line-of-site to 
    % and at what angle
    deltas = nodes(interiorNodes,:)-repmat(nodes(i,:),nInteriorNodes,1);
    
    closestThetas = wrapTo2Pi(atan2(deltas(:,2),deltas(:,1)));
    closestThetas = closestThetas*(nAngles-1)/(2*pi);
    closestThetas = round(closestThetas)+1;
    
    out = lineSegmentIntersect([repmat(nodes(i,:),nInteriorNodes,1) nodes(interiorNodes,:)],...
        map);
    
    
    nodeWalls = [];
    if(nodeTypes(i,1) == 1)
        wallDeltas = repmat(nodes(i,:),size(wallEndPoints,1),1)-wallEndPoints(:,2:3);
        dists = sqrt(sum(wallDeltas.^2,2));
        nodeWalls = wallEndPoints(dists < 1e-3,[1,4]);
    elseif(nodeTypes(i,1) == 2)
        nodeWalls = nodeTypes(i,2);
    end
    
    if(size(nodeWalls,1) > 0)
        out.intAdjacencyMatrix(:,nodeWalls(:,1)) = 0;
    end
    
    nIntersections = sum(out.intAdjacencyMatrix,2);
    
    cInteriorNodes = interiorNodes(nIntersections == 0);
    deltas = deltas(nIntersections == 0,:);
    closestThetas = closestThetas(nIntersections == 0);
    
    if(nodeTypes(i,1) == 3)
        badIndex = find(cInteriorNodes == i);
        cInteriorNodes(badIndex) = [];
        deltas(badIndex) = [];
        closestThetas(badIndex) = [];
    end
    
    srcIdxs = (i-1)*nAngles+closestThetas;
    destinations(srcIdxs) = (cInteriorNodes-1)*nAngles+closestThetas;
    
    
    % first handle wall nodes with tip angles that point into the wall
    if(nodeTypes(i,1) == 1 || nodeTypes(i,1) == 2)
        for j=1:size(nodeWalls,1)
            % wall corner or mid-point node
            wallTangent = map(nodeWalls(j,1),3:4)-map(nodeWalls(j,1),1:2);
            wallTangent = wallTangent/norm(wallTangent);
            
            wallAngle = wrapTo2Pi(atan2(wallTangent(2),wallTangent(1)));
            
            if(nodeTypes(i,1) == 1)
                % corner node
                if(nodeWalls(j,2) == 1) 
                    % first node in the wall so left is toward interior
                    minAngle = wallAngle-pi/2;
                    if(minAngle > 0)
                        angles = thetas(minAngle < thetas & thetas < wallAngle);
                    else
                        angles = thetas(thetas < wallAngle);
                        angles = vertcat(angles, thetas(thetas > 2*pi+minAngle));
                    end
                else
                    % second node in the wall so right is toward interior
                    wallAngle = wrapTo2Pi(wallAngle-pi);
                    maxAngle = wallAngle+pi/2;
                    if(maxAngle < 2*pi)
                        angles = thetas(wallAngle < thetas & thetas < maxAngle);
                    else
                        maxAngle = maxAngle-2*pi;
                        angles = thetas(wallAngle < thetas);
                        angles = vertcat(angles, thetas(thetas < maxAngle));
                    end
                end
                
                % now map destination to the other node
                % so find out what the other node is
                if(nodeWalls(j,2) == 1)
                    otherNode = map(nodeWalls(j,1),3:4);
                    otherAngle = wallAngle;
                else
                    otherNode = map(nodeWalls(j,1),1:2);
                    otherAngle = wallAngle;
                end
                
                otherNodeIdx = NodePosToIdx(otherNode, nodes);
                srcIdxs = NodeAngleToIdx(i*ones(length(angles),1), angles, nAngles); 
                destinations(srcIdxs) = NodeAngleToIdx(otherNodeIdx, otherAngle, nAngles);
            else
                % mid point node
                
                % first we will handle angles that will push you toward 
                % second node in wall
                minAngle = wallAngle-pi/2;
                if(minAngle > 0)
                    angles = thetas(minAngle < thetas & thetas < wallAngle);
                else
                    angles = thetas(thetas < wallAngle);
                    angles = vertcat(angles, thetas(thetas > 2*pi+minAngle));
                end
                otherNode = map(nodeWalls(j,1),3:4);
                otherNodeIdx = NodePosToIdx(otherNode, nodes);
                srcIdxs = NodeAngleToIdx(i*ones(length(angles,1)), angles, nAngles); 
                destinations(srcIdxs) = NodeAngleToIdx(otherNodeIdx, wallAngle);
                
                % now we will handle angles that push you toward first node
                % in wall
                wallAngle = wrapTo2Pi(wallAngle-pi);
                maxAngle = wallAngle+pi/2;
                if(maxAngle < 2*pi)
                    angles = thetas(wallAngle < thetas & thetas < maxAngle);
                else
                    maxAngle = maxAngle-2*pi;
                    angles = thetas(wallAngle < thetas);
                    angles = vertcat(angles, thetas(thetas < maxAngle));
                end
                otherNode = map(nodeWalls(j,1),1:2);
                otherNodeIdx = NodePosToIdx(otherNode, nodes);
                srcIdxs = NodeAngleToIdx(i*ones(length(angles,1)), angles, nAngles); 
                destinations(srcIdxs) = NodeAngleToIdx(otherNodeIdx, wallAngle);
                
            end
        end
    end
    
    % HERE WE ARE
    for j=1:nAngles
    end
end

%%
% check that the handedness is right
h1 = scatter(0,0,'k');
h2 = scatter(0,0,'r');
% now let's figure out where each node goes
for i=1:size(map,1)
    display(i);
    set(h1,'XData',map(i,1), 'YData', map(i,2));
    set(h2,'XData',map(i,3), 'YData', map(i,4));
    pause(0.5);
end