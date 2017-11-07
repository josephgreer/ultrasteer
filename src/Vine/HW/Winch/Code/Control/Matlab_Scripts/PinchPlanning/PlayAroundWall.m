clear; clc; close all;

ls = ones(3,1)*100;
thetas = deg2rad([270; -45; 30]);
opposite = false;
wallTurnMag = 30;
obstacle = true;
obstacleLeft = true;
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

dl = 5;

handles.robot = [];
handles.xs = [];
for i=1:50
    state = xs(end-2:end,:).'; state = state(:);
    [xy] = FindIntersectionOfTwoLines(state(3:4),state(5:6),wall(:,1),wall(:,2));
    mn = min(wall,[],2); mx = max(wall,[],2);
    if(sum(mn <= xy-1e-3) == 2 && sum(xy+1e-3 <= mx) == 2)
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