clear; clc; close all;

initTheta = deg2rad(-50);
x = [0;0;0;0;cos(initTheta);sin(initTheta)];
y = [0;0;0;1;0];
xs = [x(3) x(4); x(5) x(6)];

thetas = [131 -45; 301 60];
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
map = vertcat(map, [-300 -200 300 -200]);
map = vertcat(map, [50 -175 350 -175]);
map = vertcat(map, [400 400 400 -200]);
map = vertcat(map, [400 -200 400 -400]);
map(1:4,[1 3]) = map(1:4,[1 3])*2.5;
DrawMap(map);

handles = DrawRobotXs(xs,-1,handles);

wallIndex = -1;

ls = [];
len = 0;
dl = 10;
for i=1:100
    [x, y, xs,wallIndex] = MoveRobotByDl(x, y, dl, map, thetas, len, wallIndex, xs);
    len = RobotLength(xs)
    handles = DrawRobotXs(xs,-1,handles);
    pause(0.1);
end
    