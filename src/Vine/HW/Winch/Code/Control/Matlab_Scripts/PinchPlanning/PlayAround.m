clear; clc; close all;

ls = ones(6,1)*200;
ls(3) = 500;
ls(4) = 100;
thetas = deg2rad([0; 60; 100; -30; 100; -100]);

figure;
grid on;

handles.robot = [];

handles = DrawRobot(thetas, ls, [0; 0], 1e-5, handles);
xlim([-500 500]);
ylim([-500 500]);
grid on;
box on;
daspect([1 1 1]);

for len=linspace(1e-4,max(cumsum(ls)),60)
    handles = DrawRobot(thetas, ls, [0; 0], len, handles);

    pause(0.1);
end