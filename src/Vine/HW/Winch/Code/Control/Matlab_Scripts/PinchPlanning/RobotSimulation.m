clear; clc; close all;

initTheta = deg2rad(20);
x = [0;0;0;0;cos(initTheta);sin(initTheta)];
y = [0;0;0;0;1];
xs = [x(3) x(4); x(5) x(6)];

figure;
grid on;
hold on;

handles.robot = [];

xlim([-500 500]);
ylim([-500 500]);
daspect([1 1 1]);
grid on;
hold on;

load map;
DrawMap(map);

handles = DrawRobotXs(xs,-1,handles);

lastWallIndex = -1;
thetas = zeros(0,2);
ls = [];
len = 0;
dl = 20;
for i=1:100
    [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dl, map, thetas, len, lastWallIndex, xs);
    len = RobotLength(xs)
    handles = DrawRobotXs(xs,-1,handles);
    pause(0.1);
end
    