clear; clc; close all;

load 'Maps/map2'
load 'Maps/nodes2'

startNode = 47;
endNode = 59;

% 
map = map/8;
nodes = nodes/8;


% load 'Maps/map1'
% load 'Maps/nodes1'
% 
% startNode = 34;
% endNode = 30;

rng(1);


addpath(genpath('Geom2d'));

h = figure;

handles.robot = [];

% xlim([-500 500]);
ylim([-35 400]);
daspect([1 1 1]);
grid on;
hold on;
box on;
hold on;
xlabel('x (cm)');
ylabel('y (cm)');
set(gca,'FontSize',12,'FontName','Times New Roman');
set(h, 'Position', 0.65*[0 0 550 480]);


DrawMap(map);

nNodes = size(nodes,1);
nAngles = 359;

connectionNormal = 1e-6;
connectionInteriorNode = 3;
connectionAngleChange = 1;
useMidPointNodes = true;

% endNode = 30;
allowableAngleRange = deg2rad(50);

nVertices = nNodes*nAngles;

% nodeType 1 = wall corner
%          2 = wall midpoint
%          3 = open space
% nodeTypes = [nodeType wallIndex] (note that if nodeType = 3, then
% wallIndex is irrelevant
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

scatter(nodes([startNode,endNode],1),nodes([startNode,endNode],2),'LineWidth',2);

% show the different types of nodes
% for i=1:3
%     currIs = nodeTypes(:,1) == i;
%     if(useMidPointNodes || i ~= 2) 
%         scatter(nodes(currIs,1),nodes(currIs,2), 'LineWidth',2);
%     end
% end

interiorNodes = find(nodeTypes(:,1) == 3);
nInteriorNodes = size(interiorNodes,1);

destinations = zeros(nNodes*nAngles,1);
% this is for extending past midpoint ndoe
secondaryDestinations = -1*ones(nNodes*nAngles,1);
weights = ones(nNodes*nAngles,1)*connectionNormal;

thetas = linspace(0,2*pi,nAngles+1).';
thetas = thetas(1:end-1);
for i=1:nNodes
    display(sprintf('%d of %d nodes',i,nNodes));
    % first figure out which interior nodes we have line-of-site to 
    % and at what angle
    deltas = nodes(interiorNodes,:)-repmat(nodes(i,:),nInteriorNodes,1);
    
    closestThetas = wrapTo2Pi(atan2(deltas(:,2),deltas(:,1)));
    closestThetas = closestThetas*(nAngles)/(2*pi);
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
    weights(srcIdxs) = connectionInteriorNode;
    
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
                destinations(srcIdxs) = -1;%NodeAngleToIdx(otherNodeIdx, otherAngle, nAngles);
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
                srcIdxs = NodeAngleToIdx(i*ones(length(angles),1), angles, nAngles); 
                destinations(srcIdxs) = NodeAngleToIdx(otherNodeIdx, wallAngle, nAngles);
                
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
                srcIdxs = NodeAngleToIdx(i*ones(length(angles),1), angles, nAngles); 
                destinations(srcIdxs) = NodeAngleToIdx(otherNodeIdx, wallAngle, nAngles);
            end
        end
    end
end

% make a new map with vertices with mid points
newMap = map;
if(useMidPointNodes)
    midNodes = find(nodeTypes(:,1) == 2);
    newMap(unique(nodeTypes(midNodes,2)),:) = [];
    
    for i=1:length(midNodes)
        newMap = vertcat(newMap, [map(nodeTypes(midNodes(i),2),1:2) nodes(midNodes(i),:)]);
        newMap = vertcat(newMap, [nodes(midNodes(i),:) map(nodeTypes(midNodes(i),2),3:4)]);
    end
end

newWallEndPoints = [linspace(1,size(newMap,1),size(newMap,1)).'  newMap(:,1:2) ones(size(newMap,1),1);...
    linspace(1,size(newMap,1),size(newMap,1)).' newMap(:,3:4) 2*ones(size(newMap,1),1)];
for i=1:nNodes
    display(sprintf('%d of %d nodes',i,nNodes));
    indices = NodeAngleToIdx(i*ones(nAngles,1),thetas,nAngles);
    indexZ = indices(1);
    indices = find(destinations(indices) == 0)+(indexZ-1);
    [~,aa] = IndexToNodeAngle(indices,nAngles,nNodes,thetas);
    
    xx = zeros(6,1);
    yy = zeros(5,1);
    for j=1:size(aa,1)
        xx = [nodes(i,:).'; nodes(i,:).';...
            nodes(i,:).'+[cos(aa(j)); sin(aa(j))]];
        yy = [0; 0; 0; 0; 1];
        
        nodeWalls = [];
        if(nodeTypes(i,1) == 1 || nodeTypes(i,1) == 2)
            wallDeltas = repmat(nodes(i,:),size(newWallEndPoints,1),1)-newWallEndPoints(:,2:3);
            dists = sqrt(sum(wallDeltas.^2,2));
            nodeWalls = newWallEndPoints(dists < 1e-3,[1,4]);
        end
        
        ignoreWalls = -1;
        if(size(nodeWalls,1) > 0)
            ignoreWalls = nodeWalls(:,1);
        end
        
        [xx,yy] = MoveRobotByDlOrUntilEndOfWall(xx,yy, newMap, ignoreWalls);
        
        nodeIdx = NodePosToIdx(xx(5:6).',nodes);
        if(nodeIdx == i)
            assert(0);
        end
        
        tipTangent = xx(5:6)-xx(3:4); tipTangent = tipTangent/norm(tipTangent);
        tipAngle = wrapTo2Pi(atan2(tipTangent(2),tipTangent(1)));
        
        if(nodeIdx > 0)
            destinations(indices(j)) = NodeAngleToIdx(nodeIdx,tipAngle,nAngles);
        else
            destinations(indices(j)) = -1;
        end
        
        if(nodeIdx > 0 && nodeTypes(nodeIdx,1) == 2)
            % lets handle the case here figure out what to do
            [xx,yy] = MoveRobotByDlOrUntilEndOfWall(xx,yy, newMap, ignoreWalls);
        
            tipTangent = xx(5:6)-xx(3:4); tipTangent = tipTangent/norm(tipTangent);
            tipAngle = wrapTo2Pi(atan2(tipTangent(2),tipTangent(1)));
            
            nodeIdx = NodePosToIdx(xx(5:6).',nodes);
            if(nodeIdx > 0)
                secondaryDestinations(indices(j)) = NodeAngleToIdx(nodeIdx,tipAngle,nAngles);
            end
        end
    end
end

rowColVal = zeros((nVertices+2)*2*nAngles,3);
nVals = 0;

for j=1:length(thetas)
    display(sprintf('Theta %d of %d', j, length(thetas)));
    thetaUp = thetas(j) + allowableAngleRange;
    thetaDown = thetas(j) - allowableAngleRange;
    
    if(thetaUp > 2*pi)
        allowableThetas = [thetas(j:end);thetas(thetas < thetaUp-2*pi)];
    else
        allowableThetas = thetas(thetas(j) <= thetas & thetas < thetaUp);
    end
    
    if(thetaDown < 0)
        allowableThetas = [allowableThetas; thetas(1:j); thetas(thetas > 2*pi+thetaDown)];
    else
        allowableThetas = [allowableThetas; thetas(thetaDown < thetas & thetas <= thetas(j))];
    end
    
    angleDiffs = angleAbsDiff(repmat(thetas(j),size(allowableThetas,1),1),allowableThetas);
    
    for ii=1:nNodes
        allowableIndices = NodeAngleToIdx((ii)*ones(size(allowableThetas)), allowableThetas, nAngles);
        index = NodeAngleToIdx(ii,thetas(j),nAngles);
        
        rowColVal(nVals+1:nVals+length(allowableIndices),:) =...
            [repmat(index,length(allowableIndices),1) allowableIndices...
            repmat(connectionAngleChange,length(allowableIndices),1)+angleDiffs];
        nVals = nVals + length(allowableIndices);
    end
end

% now create the adjacency matrix
for i=1:nNodes
    display(sprintf('Adjacency Matrix node %d of %d nodes', i, nNodes));
    indices = NodeAngleToIdx(i*ones(nAngles,1),thetas,nAngles);
    indexZ = indices(1);
    
    normalConnectionIndices = find(destinations(indices) > 0 & weights(indices) == connectionNormal)+indexZ-1;
    rowColVal(nVals+1:nVals+length(normalConnectionIndices),:) =...
        [normalConnectionIndices destinations(normalConnectionIndices)...
        repmat(connectionNormal,length(normalConnectionIndices),1)];
    nVals = nVals + length(normalConnectionIndices);
    
    secondaryConnectionIndices = find(secondaryDestinations(indices) > 0)+indexZ-1;
    rowColVal(nVals+1:nVals+length(secondaryConnectionIndices),:) =...
        [secondaryConnectionIndices secondaryDestinations(secondaryConnectionIndices)...
        repmat(connectionNormal,length(secondaryConnectionIndices),1)];
    nVals = nVals + length(secondaryConnectionIndices);

%     [nn,aa] = IndexToNodeAngle(normalConnectionIndices,nAngles,nNodes,thetas);
    
%     A(sub2ind(size(A),normalConnectionIndices,destinations(normalConnectionIndices))) = connectionNormal;
%     turnedIndices = NodeAngleToIdx(nn+nNodes,aa,nAngles);
%     A(turnedIndices,destinations(normalConnectionIndices)) = connectionNormal;
    
    
    interiorConnectionIndices = find(destinations(indices) > 0 & weights(indices) == connectionInteriorNode)+indexZ-1;
%     [nn,aa] = IndexToNodeAngle(interiorConnectionIndices,nAngles,nNodes,thetas);
    rowColVal(nVals+1:nVals+length(interiorConnectionIndices),:) =...
        [interiorConnectionIndices destinations(interiorConnectionIndices)...
        repmat(connectionInteriorNode,length(interiorConnectionIndices),1)];
    nVals = nVals + length(interiorConnectionIndices);
    
%     A(sub2ind(size(A),interiorConnectionIndices, destinations(interiorConnectionIndices))) = connectionInteriorNode;
%     turnedIndices = NodeAngleToIdx(nn+nNodes,aa,nAngles);
%     A(turnedIndices,destinations(normalConnectionIndices)) = connectionNormal;
end

A = sparse(rowColVal(1:nVals,1),rowColVal(1:nVals,2),rowColVal(1:nVals,3),nVertices+2,nVertices+2);

% source node
startIndices = NodeAngleToIdx(startNode*ones(nAngles,1),thetas,nAngles);
A(nVertices+1,startIndices) = 1;
A(startIndices,nVertices+1) = 1;

% end node
endIndices = NodeAngleToIdx(endNode*ones(nAngles,1),thetas,nAngles);
A(nVertices+2,endIndices) = 1;
A(endIndices,nVertices+2) = 1;

display('Calculating shortest path');
[dist, path] = graphshortestpath(A,nVertices+1,nVertices+2);

% for i=2:length(path)-2
%     [ns,as] = IndexToNodeAngle(path(i:i+1),nAngles,nNodes,thetas);
%     ns = nodes(ns,:);
%     plot(ns(:,1),ns(:,2));
% end

nodeList = zeros(length(path)-2,1);
for i=2:length(path)-1
    [ns,as] = IndexToNodeAngle(path(i),nAngles,nNodes,thetas);
    nodeList(i-1) = ns;
    display(sprintf('node %d exit angle %f', ns(1), rad2deg(as(1))));
end

% this contains the list of nodes we will visit on our way to a destination
nodeList = unique(nodeList,'stable');

allowableAngleRange = deg2rad(50);
angleNoise = deg2rad(10); % in degree uniform distribution
lengthNoise = 10; % in physical units
nSamples = 100; % number of samples used to calculate monte carlo prboabilities
nAngles = 100;

thetas = linspace(0,2*pi,nAngles+1).';
thetas = thetas(1:end-1);
thetas = wrapToPi(thetas);

lengthPadding = 20;

validThetas = thetas(abs(thetas) < allowableAngleRange);

rx = {[nodes(nodeList(1),:).'; nodes(nodeList(1),:).';...
    nodes(nodeList(1),:).']};
ry = {[0; 0; 0; 0; 1]};
rxs = {[rx{1}(1:2:end) rx{1}(2:2:end)]};
rwi = {-1};

designLs = [0];
designThetas = [];

for i=1:length(nodeList)-1
    preNode = nodeList(i);
    desNode = nodeList(i+1);
    
    nodeWalls = [];
    if(nodeTypes(preNode,1) == 1 || nodeTypes(preNode,1) == 2)
        wallDeltas = repmat(nodes(preNode,:),size(newWallEndPoints,1),1)-newWallEndPoints(:,2:3);
        dists = sqrt(sum(wallDeltas.^2,2));
        nodeWalls = newWallEndPoints(dists < 1e-3,[1,4]);
    end
    
    cValidThetas = validThetas;
    if(i == 1)
        cValidThetas = thetas;
    end
        
    if(nodeTypes(desNode,1) == 1 || nodeTypes(desNode,1) == 2)
        % holds success/fail (1 = success, 0 = fail) for each theta and each
        % sample at arriving at next node
        success = zeros(length(cValidThetas),length(rx));
        
        lens = zeros(length(cValidThetas), length(rx));
    
        % for each angle turn
        for j = 1:length(cValidThetas)
            theta = cValidThetas(j);
            display(sprintf('Landmark %d, Angle %d of %d',i,j,length(cValidThetas)));
            
            for k = 1:length(rx)
                x = rx{k};
                y = ry{k};
                xs = rxs{k};
                wi = rwi{k};
                
                % find all configurations that end at node nodeList(i+1)
                try
                    [xx,yy,xxs,wi] = MoveRobotUntilNodeEncountered(x,y,xs,wi,newMap,theta,nodes,desNode);
                catch
                    display('you suck');
                    yy(5) = 1e6;
                end
                if(yy(5) ~= 1e6)
                    % we arrived at the right place
                    % j is angle, k is sample
                    success(j,k) = 1;
                    lens(j,k) = RobotLength(xxs);
                end
            end
        end
        
        % probability of success for each angle  (marignalized over
        % samples)
        successProb = mean(success,2);
        
        % success of each sample but also taking into account other thetas
        successProbWithUncertaintiy = zeros(length(cValidThetas),1);
        
        % lens
        lensWithUncertainty = zeros(length(cValidThetas),1);
        
        % now find probability when including other angles into the mix
        for j=1:length(cValidThetas)
            relevantProbs = [];
            
            sumIdxs = [];
            if(i > 1)
                if(cValidThetas(j) == 0)
                    successProbWithUncertaintiy(j) = successProb(j);
                    lensWithUncertainty(j) = max(max(lens(j,:)));
                    % there is no added uncertainty if we are not turning
                    continue;
                end
                
                if(cValidThetas(j) < 0)
                    % right turn, never going to accidentally get a left turn
                    lowerLim = cValidThetas(j)-angleNoise;
                    upperLim = min(cValidThetas(j)+angleNoise,0);
                else
                    % left turn, never going to accidentally get a right turn
                    lowerLim = max(cValidThetas(j)-angleNoise,0);
                    upperLim = cValidThetas(j)+angleNoise;
                end
                sumIdxs = find(lowerLim < cValidThetas & cValidThetas < upperLim);
            else
                lowerLim = cValidThetas(j)-angleNoise;
                if(lowerLim < -pi)
                    sumIdxs = find(cValidThetas <= cValidThetas(j));
                    sumIdxs = vertcat(sumIdxs, find(2*pi+lowerLim < cValidThetas));
                else
                    sumIdxs = find(lowerLim <= cValidThetas & cValidThetas <= cValidThetas(j));
                end
                
                upperLim = cValidThetas(j)+angleNoise;
                if(upperLim > pi)
                    sumIdxs = vertcat(sumIdxs,find(cValidThetas(j) <= cValidThetas));
                    sumIdxs = vertcat(sumIdxs,find(cValidThetas <= upperLim-2*pi));
                else
                    sumIdxs = vertcat(sumIdxs,find(cValidThetas(j) <= cValidThetas & cValidThetas <= upperLim));
                end
            end
            sumIdxs = unique(sumIdxs);
            
            % assumes uniform angular uncertainty
            lensWithUncertainty(j) = max(max(lens(sumIdxs,:)));
            successProbWithUncertaintiy(j) = mean(successProb(sumIdxs,:));
        end
        
        % best angle is arg_max successProbWithUncertainty
        [~, optJ] = max(successProbWithUncertaintiy);
        if(abs(cValidThetas(optJ)) > 0)
            designThetas = vertcat(designThetas, cValidThetas(optJ));
            designLs = vertcat(designLs, lensWithUncertainty(optJ)+lengthPadding);
        else
            designLs(end) = lensWithUncertainty(optJ)+lengthPadding;
        end
    elseif(nodeTypes(desNode,1) == 3)
        % interior node. for each sample, check if there is an unobstructed
        % ray to destination node
        
        rrx = cell2mat(rx);
        
        proxTangent = rrx(5:6,:)-rrx(3:4,:);
        proxTangent = mean(proxTangent,2);
        proxTangent = proxTangent/norm(proxTangent);
        
        deltas = repmat(nodes(desNode,:).',1,size(rrx,2))-rrx(5:6,:);
        rls = sqrt(sum(deltas.^2,1));
        delta = mean(deltas,2);
        delta = delta/norm(delta);
        
        ctheta = angleDiffSigns([proxTangent;0].',[delta;0].');
        
        designThetas = vertcat(designThetas,ctheta);
        designLs = vertcat(designLs,mean(rls)+designLs(end));
    end
    
    % now simulate a bunch of paths using this angle
        
    thetaRight = find(designThetas < 0);
    thetaLeft = find(designThetas >= 0);
    
    k = 1;
    nsucc = 0;
    nfail = 0;
    while(k < nSamples)
        cls = designLs+unifrnd(-lengthNoise,lengthNoise,length(designLs),1);
        cls = max(cls,1e-3);
        cls(1) = 0;
        
        thetaPerturb = unifrnd(-angleNoise,angleNoise,length(designThetas),1);
        cts = designThetas+thetaPerturb;
        cts(thetaRight) = min(cts(thetaRight),-1e-3);
        cts(thetaLeft) = max(cts(thetaLeft),1e-3);
        
        
        cx = [nodes(nodeList(1),:).'; nodes(nodeList(1),:).';...
            nodes(nodeList(1),:).'];
        cy = [0; 0; 0; 0; 1];
        cxs = [cx(1:2:end) cx(2:2:end)];
        
        clen = 0;
        cdls = cls(2:end)-cls(1:end-1);
        cthetas = [cls(1:end-1) cts];
        wallIndex = -1;
        for ll=1:length(cdls)
            [cx, cy, cxs,wallIndex] = MoveRobotByDl(cx,cy,cdls(ll),newMap,cthetas,clen,wallIndex,cxs);
            
            clen = clen+cdls(ll);
        end
        
        if(nodeTypes(desNode,1) == 1 || nodeTypes(desNode,1) == 2)
            if(norm(cy(1:2)-nodes(desNode,:).') < 1e-3 || norm(cx(5:6) - nodes(desNode,:).') < 50)
                rx{k} = cx;
                ry{k} = cy;
                rxs{k} = cxs;
                rwi{k} = wallIndex;
                k = k+1;
                nsucc = nsucc+1;
            else
                nfail = nfail+1;
            end
        else 
            if(norm(cx(5:6) - nodes(desNode,:).') < 60)
                rx{k} = cx;
                ry{k} = cy;
                rxs{k} = cxs;
                rwi{k} = wallIndex;
                nsucc = nsucc+1;
                k = k+1;
            else
                nfail = nfail+1;
            end
        end
    end
    display(sprintf('Success estimate at landmark %d is %f', i, nsucc/(nfail+nsucc)));
end


angleNoises = deg2rad(linspace(0,25,12));
successProbsOptimalDesign = zeros(length(angleNoises),1);
for kk = 1:length(angleNoises)
    nTries = 1000;
    
    handles.robot = [];
    nfail = 0;
    nsucc = 0;
    thetaRight = find(designThetas < 0);
    thetaLeft = find(designThetas >= 0);
    
    
    tipLocations = zeros(nTries,2);
    
    angleNoise = angleNoises(kk);
    for jj=1:nTries
        cls = designLs+unifrnd(-lengthNoise,lengthNoise,length(designLs),1);
        cls = max(cls,1e-3);
        cls(1) = 0;
        
        thetaPerturb = unifrnd(-angleNoise,angleNoise,length(designThetas),1);
        cts = designThetas+thetaPerturb;
        cts(thetaRight) = min(cts(thetaRight),-1e-3);
        cts(thetaLeft) = max(cts(thetaLeft),1e-3);
        
        thetas = [cls(1:end-1) cts];
        
        xx = [nodes(startNode,:).'; nodes(startNode,:).';...
            nodes(startNode,:).'];
        yy = [0; 0; 0; 0; 1];
        xxs = [xx(1:2:end) xx(2:2:end)];
        
        dls = cls(2:end)-cls(1:end-1);
        
        wallIndex = -1;
        if(kk == 1 && jj == 1)
            cmap = zeros(0,4);
        else
            cmap = map;
        end
        for i=1:length(dls)
            if(jj == 1 || jj == 2)
                for j=1:10
                    [xx, yy, xxs,wallIndex] = MoveRobotByDl(xx, yy, dls(i)/10, cmap, thetas, RobotLength(xxs), wallIndex, xxs);
                    handles = DrawRobotXs(xxs,-1,handles);
                    pause(0.25);
                end
            else
                [xx, yy, xxs,wallIndex] = MoveRobotByDl(xx, yy, dls(i), cmap, thetas, RobotLength(xxs), wallIndex, xxs);
            end
        end
        if(kk == 1 && (jj == 1 || jj == 2))
            ScaleDataPlot(8/90*2.54);
            handles = DrawRobotXs(xxs, -1, handles);
        end
        if(norm(xx(5:6) - nodes(endNode,:).') < 20)
            nsucc = nsucc+1;
        else
            nfail = nfail+1;
        end
        tipLocations(jj,:) = xx(5:6).';
    end
    
%     scatter(tipLocations(:,1),tipLocations(:,2));
    
    successProb = nsucc/(nsucc+nfail);
    successProbsOptimalDesign(kk) = successProb;
    display(sprintf('Optimal Design Prob Success %f AngleNoise %f', successProb, rad2deg(angleNoise)));
end

load('Maps/NominalDesign2');

successProbsNominalDesign = zeros(length(angleNoises),1);

for kk = 1:length(angleNoises)
    nTries = 1000;
    
    handles.robot = [];
    nfail = 0;
    nsucc = 0;
    thetaRight = find(designThetas < 0);
    thetaLeft = find(designThetas >= 0);
    
    
    tipLocations = zeros(nTries,2);
    
    
    angleNoise = angleNoises(kk);
    for jj=1:nTries
        cls = designLs+unifrnd(-lengthNoise,lengthNoise,length(designLs),1);
        cls = max(cls,1e-3);
        cls(1) = 0;
        
        thetaPerturb = unifrnd(-angleNoise,angleNoise,length(designThetas),1);
        cts = designThetas+thetaPerturb;
        cts(thetaRight) = min(cts(thetaRight),-1e-3);
        cts(thetaLeft) = max(cts(thetaLeft),1e-3);
        
        thetas = [cls(1:end-1) cts];
        
        xx = [nodes(startNode,:).'; nodes(startNode,:).';...
            nodes(startNode,:).'];
        yy = [0; 0; 0; 0; 1];
        xxs = [xx(1:2:end) xx(2:2:end)];
        
        dls = cls(2:end)-cls(1:end-1);
        
        
        if(false)%kk == 1 && jj == 1)
            cmap = zeros(0,4);
        else
            cmap = map;
        end
        
        wallIndex = -1;
        badOne = false;
        for i=1:length(dls)
            try
                [xx, yy, xxs,wallIndex] = MoveRobotByDl(xx, yy, dls(i), cmap, thetas, RobotLength(xxs), wallIndex, xxs);
            catch
                badOne = true;
                break;
            end
        end
        if(badOne)
            nfail = nfail+1;
            tipLocations(jj,:) = [0 0];
%             DrawRobotXs(xxs,-1,handles);
            continue;
        else
            tipLocations(jj,:) = xx(5:6).';
        end
        if(kk == 1 && (jj == 1 || jj == 2))
            handles = DrawRobotXs(xxs, -1, handles);
            ScaleDataPlot(8/90*2.54);
            break;
        end
        
        if(norm(xx(5:6) - nodes(endNode,:).') < 20)
            nsucc = nsucc+1;
        else
            nfail = nfail+1;
        end
    end
    successProb = nsucc/(nsucc+nfail);
    successProbsNominalDesign(kk) = successProb;
    display(sprintf('Nominal Design Prob Success %f Angle Noise %f', successProb, rad2deg(angleNoise)));
end

h = figure;
set(gca,'FontSize',12,'FontName','Times New Roman');
set(h, 'Position',[0 0 400 210]);
hold all;
box on;
grid on;
plot(rad2deg(angleNoises), successProbsOptimalDesign, '-o', 'LineWidth',2);
plot(rad2deg(angleNoises), successProbsNominalDesign, '-o', 'LineWidth', 2);
xlabel('Angular Uncertainty (\circ)');
ylabel('Probability of Success');
legend('Optimal Design', 'Nominal Design');

% %%
% % check that the handedness is right
% h1 = scatter(0,0,'k');
% h2 = scatter(0,0,'r');
% % now let's figure out where each node goes
% for i=1:size(map,1)
%     display(i);
%     set(h1,'XData',map(i,1), 'YData', map(i,2));
%     set(h2,'XData',map(i,3), 'YData', map(i,4));
%     pause(0.5);
% end