clear; clc; close all;

ls = ones(3,1)*100;
thetas = deg2rad([200; -30; 45]);
opposite = false;
wallTurnMag = 20;
obstacle = true;
obstacleLeft = true;

if(opposite)
    wallTangentRotate = deg2rad(-sign(thetas(end))*wallTurnMag);
else
    wallTangentRotate = deg2rad(sign(thetas(end))*wallTurnMag);
end

figure;
grid on;

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

wall = repmat(wallIntersectionPoint,1,2)+[300*wallTangent -300*wallTangent];

plot(wall(1,:), wall(2,:), 'k', 'LineWidth', 2);

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

handles.robot = [];
for i=1:50
    state = xs(end-2:end,:).'; state = state(:);
    [newX, newY, newState] = MoveRobotForwardAlongWall(state, y, wall, 5);
    
    display(norm(newX(end-1:end)-newX(end-3:end-2)));
    if(newState)
        xs(end-1:end,:) = reshape(newX(1:4),2,2).';
        xs = [xs; [newX(end-1) newX(end)]];
    else
        xs(end-2:end,:) = reshape(newX,2,3).';
    end
    
    y = newY;
    
    handles = DrawRobotXs(xs,sum(ls)+i*5,handles);
    pause(0.1);
end