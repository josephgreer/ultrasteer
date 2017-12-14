clear; clc; close all;

addpath(genpath('Geom2d'));

rng(4);

initTheta = deg2rad(unifrnd(0,360));
p0 = [-250; -100];
x = [p0;p0;p0(1)+cos(initTheta);p0(2)+sin(initTheta)];
y = [0;0;0;1;0];
xs = [x(3) x(4); x(5) x(6)];

thetas = [51 -30]; %101 30; 151 -30; 201 30; 251 -30];
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

% load Maps/map1
load map;
map(7,:) = [];
map = vertcat(map, [-300 -200 300 -200]);
map = vertcat(map, [50 -175 350 -175]);
map = vertcat(map, [400 400 400 -200]);
map = vertcat(map, [400 -200 400 -400]);
map(1:4,[1 3]) = map(1:4,[1 3])*2.5;
map = GenerateForestMap(12);
% map = map*1.75;
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
    