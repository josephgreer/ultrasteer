clear; clc; close all;

initTheta = deg2rad(20);
x = [0;0;0;0;cos(initTheta);sin(initTheta)];
y = [0;0;0;0;1];
xs = [x(3) x(4); x(5) x(6)];

thetas = [21 -30];
% thetas = zeros(0,2);
thetas(:,2) = deg2rad(thetas(:,2));

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
map(7,:) = [];
map = vertcat(map, [0 -200 300 -200]);
map = vertcat(map, [50 -175 300 -175]);
map = vertcat(map, [400 400 400 -200]);
map = vertcat(map, [400 -200 400 -400]);
DrawMap(map);

handles = DrawRobotXs(xs,-1,handles);

wallIndex = -1;

ls = [];
len = 0;
dl = 20;
for i=1:100
    [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dl, map, thetas, len, wallIndex, xs);
    len = RobotLength(xs)
    handles = DrawRobotXs(xs,-1,handles);
    pause(0.1);
end
    