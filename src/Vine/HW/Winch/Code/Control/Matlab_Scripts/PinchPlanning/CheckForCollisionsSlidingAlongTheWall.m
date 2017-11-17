function [intersects,x,y,dl,xs] = CheckForCollisionsSlidingAlongTheWall(xstart,xend,y,dir,b,wallIndex,walls,xs)
% in this case we're sliding along a wall with in the direction of the
% most distal turn

% there is one way to collide with an obstacle:
% the proximal segment can collide with an obstacle end-point

x = xstart;

intersects = false;
y = y;
dl = 0;

l1Sq = xend(5:6)-xend(3:4);
l1Sq = dot(l1Sq,l1Sq);
l1 = sqrt(l1Sq);

p0 = xend(3:4);

a1 = wrapTo2Pi(atan2(xend(4)-xend(2),xend(3)-xend(1)));
a0 = wrapTo2Pi(atan2(xstart(4)-xstart(2),xstart(3)-xstart(1)));

wallEndPoints = vertcat(walls(:,1:2), walls(:,3:4));
origWallEndPoints = wallEndPoints;

wallEndPoints = wallEndPoints-repmat([p0(1),p0(2)],size(wallEndPoints,1),1);
wallEndPoints = [linspace(1,size(wallEndPoints,1),size(wallEndPoints,1)).' wallEndPoints];

distSq = sum(wallEndPoints(:,2:3).^2,2);

% check for case 1
possibleInnerIntersections = wallEndPoints(find(distSq <= l1Sq),:);

innerThetas = [possibleInnerIntersections(:,1)...
    wrapTo2Pi(atan2(possibleInnerIntersections(:,3), possibleInnerIntersections(:,2)))];

% innerThetas are the angles at which case 1 intersections will ocurr
innerThetas = innerThetas(find(min([a0 a1]) <= innerThetas(:,2) & innerThetas(:,2) <= max([a0 a1])),:);

finalThetas = [innerThetas origWallEndPoints(innerThetas(:,1),:) ones(length(innerThetas),1)];

%now find the first obstacle and handle appropriately
finalThetas(:,1) = wrapTo2Pi(finalThetas(:,1));
a0 = wrapTo2Pi(a0);

angleDiffs = abs(finalThetas(:,1)-a0);
angleDiffs = min(angleDiffs,2*pi-angleDiffs);

[minTheta,minThetaIdx] = min(angleDiff(finalThetas(:,1),repmat(a0,size(finalThetas,1),1)));

oldLen = RobotLength(xs);
if(~isempty(minTheta))
    intersects = true;
    minTheta = finalThetas(minThetaIdx,1);
    
    x(3:4) = finalThetas(minThetaIdx, 2:3).';
    x(5:6) = x(3:4)+[cos(minTheta);sin(minTheta)]*1e7;
    
    out = lineSegmentIntersect([x(3:6).'], walls(wallIndex,:));
    x(5) = out.intMatrixX;
    x(6) = out.intMatrixY;
    
    xs(end,:) = [x(3) x(4)];
    xs = vertcat(xs,[x(5) x(6)]);
    
    newLen = RobotLength(xs);
    dl = newLen-oldLen;
end

end