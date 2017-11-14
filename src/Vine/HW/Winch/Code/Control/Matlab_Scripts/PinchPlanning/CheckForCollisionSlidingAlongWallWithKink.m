function [intersects,x,y] = CheckForCollisionSlidingAlongWallWithKink(xstart,xend,y,dir,b,wallIndex,walls)
% in this case we're sliding along a wall with in the opposite direction of
% the most distal turn

% there are three ways colid with an obstacle
% 1) the proximal segment can collide with an obstacle end-point
% 2) the distal segment can collide with an obstacle end-point
% 3) the "elbow" of the robot can collide with a wall section

intesrsects = false;

l3Sq = xend(5:6)-xend(1:2);
l3Sq = dot(l3Sq,l3Sq);

l1Sq = xend(3:4)-xend(1:2);
l1Sq = dot(l1Sq,l1Sq);
l1 = sqrt(l1Sq);

p0 = xend(1:2);

a1 = wrapTo2Pi(atan2(xend(4)-xend(2),xend(3)-xend(1)));
a0 = wrapTo2Pi(atan2(xstart(4)-xstart(2),xstart(3)-xstart(1)));

wallEndPoints = vertcat(walls(:,1:2), walls(:,3:4));

wallEndPoints = wallEndPoints-repmat([p0(1),p0(2)],size(wallEndPoints,1),1);
wallEndPoints = [linspace(1,size(wallEndPoints,1),size(wallEndPoints,1)).' wallEndPoints];

distSq = sum(wallEndPoints(:,2:3).^2,2);

% check for case 1
possibleInnerIntersections = wallEndPoints(find(distSq <= l1Sq),:);

innerThetas = [possibleInnerIntersections(:,1)...
    wrapTo2Pi(atan2(possibleInnerIntersections(:,3), possibleInnerIntersections(:,2)))];

% innerThetas are the angles at which case 1 intersections will ocurr
innerThetas = innerThetas(find(min([a0 a1]) <= innerThetas(:,2) & innerThetas(:,2) <= max([a0 a1])),:);

finalThetas = [innerThetas wallEndPoints(innerThetas(:,1),2:3) ones(length(innerThetas),1)];

% check for case 2
possibleOuterInteractions = wallEndPoints(find(l1Sq < distSq & distSq <= l3Sq),:);

l3s = sqrt(sum(possibleOuterInteractions(:,2:3).^2,2));
l3Theta = wrapTo2Pi(atan2(possibleOuterInteractions(:,3),possibleOuterInteractions(:,2)));

b = pi-b;
cs = l1*sin(b)./l3s;

as = pi-cs-b;

if(dir > 0)
    outerThetas = l3Theta+as;
else
    outerThetas = l3Theta-as;
end
outerThetas = wrapTo2Pi(outerThetas);

outerStuff = [possibleOuterInteractions(:,1) outerThetas l3s as];
outerStuff = outerStuff(find(min([a0 a1]) <= outerStuff(:,2) & outerStuff(:,2) <= max([a0 a1])),:);

sinas = sin(outerStuff(:,4));

l2s = outerStuff(:,3).*sinas/sin(b);

l1Vecs = l1*[cos(outerStuff(:,2)) sin(outerStuff(:,2))]+repmat(p0.',size(outerStuff,1),1);
l3Vecs = wallEndPoints(outerStuff(:,1),2:3);

if(dir == 1)
    angles = outerStuff(:,2)-(pi-b);
    l3Vecs = l1Vecs+repmat(l2s,1,2).*[cos(angles) sin(angles)];
else
    angles = outerStuff(:,2)+(pi-b);
    l3Vecs = l1Vecs+repmat(l2s,1,2).*[cos(angles) sin(angles)];
end


outerLineSegs = [l1Vecs l3Vecs];
out = lineSegmentIntersect(outerLineSegs, walls(wallIndex,:));

% outerStuff holds the information about the distal segment intersections
outerStuff(find(out.intAdjacencyMatrix > 0),:) = [];
finalThetas = vertcat(finalThetas,[outerStuff(:,2) wallEndPoints(outerStuff(:,1),2:3)...
    2*ones(size(outerStuff,1),1)]);


% check case 3
lines = [walls(:,1:2) walls(:,3:4)-walls(:,1:2)];
circles = repmat([p0(1) p0(2) l1], size(lines,1),1);
[validElbowPoints,validIdxs] = intersectLineCircle(lines,circles);
validIdxs = [validIdxs;validIdxs];

shiftedElbowPoints = validElbowPoints-repmat(p0.',size(validElbowPoints,1),1);

elbowThetas = wrapTo2Pi(atan2(shiftedElbowPoints(:,2),shiftedElbowPoints(:,1)));

validPoints = linspace(1,size(elbowThetas,1),size(elbowThetas,1)).';
validPoints = find(min([a0 a1]) <= elbowThetas & elbowThetas <= max([a0 a1]));
validElbowPoints = validElbowPoints(validPoints,:);
validIdxs = validIdxs(validPoints);

values = CheckPointsBetweenLineSegments(validElbowPoints,walls(validIdxs,:));

% this holds the information about the elbow point intersections

validIdxs = find(values > 0);
finalThetas = vertcat(finalThetas,[elbowThetas(validIdxs) validElbowPoints(validIdxs, :)...
    2*ones(length(validIdxs),1)]);
finalThetas(:,1) = wrapTo2Pi(finalThetas(:,1));
a0 = wrapTo2Pi(a0);

angleDiffs = abs(finalThetas(:,1)-a0);
angleDiffs = min(angleDiffs,2*pi-angleDiffs);

[minTheta,minThetaIdx] = min(angleDiff(finalThetas(:,1),repmat(a0,size(finalThetas,1),1)));

if(~isempty(minTheta))
    intersects = true;
    % now reconstruct the state
    l1Vec = l1*[cos(minTheta); sin(minTheta)]+p0;
    
    if(dir == 1)
        angle = minTheta-(pi-b);
        l3Vec = l1Vec+1e6*[cos(angle); sin(angle)];
    else
        angle = minTheta+(pi-b);
        l3Vec = l1Vec+1e6*[cos(angle); sin(angle)];
    end
    out = lineSegmentIntersect([l1Vec.' l3Vec.'], walls(wallIndex,:));
    x(5) = out.intMatrixX;
    x(6) = out.intMatriY;
    
    if(finalThetas(minThetaIdx,end) == 2)
        % distal intersection
        x(1:2) = x(3:4);
        x(3:4) = finalThetas(minThetaIdx,2:3);
        if(dir == 1)
            y(5) = 0;
            y(4) = 1;
        else
            y(5) = 1;
            y(4) = 0;
        end
    else
        % proximal intersection
        x(1:2) = finalThetas(minThetaIdx,2:3);
    end
end

end