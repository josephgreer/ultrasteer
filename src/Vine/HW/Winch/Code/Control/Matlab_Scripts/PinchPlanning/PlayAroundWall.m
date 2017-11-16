clear; clc; close all;

ls = ones(3,1)*100;
thetas = deg2rad([180; 45; -30]);
opposite = true;
wallTurnMag = 45;
obstacle = false;
obstacleLeft = false;
proximalObstacle = true;
proximalObstacleLeft = true;
wallLength = 200;

if(opposite)
    wallTangentRotate = deg2rad(-sign(thetas(end))*wallTurnMag);
else
    wallTangentRotate = deg2rad(sign(thetas(end))*wallTurnMag);
end

figure;
grid on;
hold on;

p0 = [0; 0];

handles.robot = [];
handles = DrawRobot(thetas,ls,p0, sum(ls), handles);

xlim([-500 500]);
ylim([-500 500]);
daspect([1 1 1]);
grid on;
hold on;

xs = thetasLsToXs(thetas, ls, p0);

tipSeg = (xs(end,:)-xs(end-1,:)).'; tipSeg = tipSeg/norm(tipSeg);

wallIntersectionPoint = xs(end-1,:).'+200*tipSeg;
wallTangent = PlaneRotation(wallTangentRotate)*tipSeg;

wall = repmat(wallIntersectionPoint,1,2)+[0.5*wallLength*wallTangent -0.5*wallLength*wallTangent];

load map;
map = vertcat(map,[wall(:,1).' wall(:,2).']);
wallIndex = size(map,1);

if(proximalObstacle)
    proximalWall = xs(end-1,:).'+150*tipSeg;
    proximalWall = [proximalWall xs(end-1,:).'+220*tipSeg];
    
    if(~proximalObstacleLeft)
        offset = [-wallLength*wallTangent -0.15*wallLength*wallTangent]
    else
        offset = [wallLength*wallTangent 0.15*wallLength*wallTangent];
    end
    proxWall = kron(proximalWall,[1 1])+repmat(offset,1,size(proximalWall,2));
    for i=1:size(proxWall,2)/2
        map = vertcat(map,[proxWall(:,2*(i-1)+1).' proxWall(:,2*(i-1)+2).']);
    end
end
% map = vertcat(map,[-200 -50 200 -50]);
% map = vertcat(map,[-200 -90 200 -90]);

DrawMap(map);

y = zeros(5,1);
if(sign(thetas(end-1)) > 0)
    y(4) = 0;
else
    y(4) = 1;
end

if(sign(thetas(end)) > 0)
    y(5) = 0;
else
    y(5) = 1;
end

if(obstacle)
        obstaclePoint = xs(end,:).'-35*tipSeg;
        
        y(3) = y(4);
        y(4) = y(5);
        if(obstacleLeft)
            y(5) = 2;
        else
            y(5) = 3;
        end
        y(1:2) = obstaclePoint;
        
        scatter(obstaclePoint(1), obstaclePoint(2), 'k', 'LineWidth', 2);
end

dl = 200;

handles.robot = [];
handles.xs = [];
for i=1:50
    state = xs(end-2:end,:).'; state = state(:);
    tipTangent = (xs(end,:)-xs(end-1,:)).'; tipTangent = tipTangent/norm(tipTangent);
    
    testPoint = state(5:6)+dl*tipTangent;
    
    out = lineSegmentIntersect([state(3) state(4) testPoint(1) testPoint(2)], [wall(1,1) wall(2,1) wall(1,2) wall(2,2)]);
    intersected = false;
    if(out.intAdjacencyMatrix > 0)
        intersected = true;
        intersectionPoint = [out.intMatrixX; out.intMatrixY];
        deltas = repmat(intersectionPoint,1,2)-wall;
        if(min(sum(deltas.^2,1)) < 1e-4)
            intersected = false;
        end
    end
    
    if(intersected)
        [newX, newY, xs, newState] = MoveRobotForwardAlongWall(state, y, wallIndex, dl, map, xs);
    else
        tipTangent = state(5:6)-state(3:4); tipTangent = tipTangent/norm(tipTangent);
        state(5:6) = state(5:6)+tipTangent*dl;
        newX = state;
        xs(end,:) = [state(5) state(6)];
        newY = y;
    end
    
    deltas = xs(2:end,:)-xs(1:end-1,:); newls = sqrt(sum(deltas.^2,2)); display(sum(newls));
    
    y = newY;
    
    handles = DrawRobotXs(xs,sum(ls)+i*dl,handles);
%     if(isempty(handles.xs))
%         handles.xs = scatter(xs(:,1),xs(:,2),'LineWidth',5);
%     else
%         set(handles.xs,'XData',xs(:,1),'YData',xs(:,2))
%     end
    pause(0.1);
end