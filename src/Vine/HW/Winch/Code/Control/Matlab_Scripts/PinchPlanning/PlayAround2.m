clear; clc; close all;

ls = 100;
thetas = deg2rad([-45]);

figure;
grid on;
hold on;

handles.robot = [];

handles = DrawRobot(thetas, ls, [0; 0], 100, handles);
xlim([-500 500]);
ylim([-500 500]);
grid on;
box on;
daspect([1 1 1]);

wall = [100*sqrt(2)/2 -200; 100*sqrt(2)/2 200];
wallT = (wall(1,:)-wall(2,:)).';
wallT = wallT/norm(wallT);

plot(wall(:,1), wall(:,2), 'r', 'LineWidth', 2);

l1 = 200;
l2 = ls(1);

a = pi/2+thetas(1);


b = pi-asin(l2/l1*sin(a));

c = pi-a-b;

thetas = c+thetas(1);
ls = 200;

handles.robot = [];
DrawRobot(thetas, ls, [0; 0], 200, handles);
