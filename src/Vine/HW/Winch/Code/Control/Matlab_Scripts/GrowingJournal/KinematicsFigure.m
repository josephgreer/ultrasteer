clear; clc; close all;

addpath('../SysID/');
addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter/');

figure;
drawVineTube([100 91 100].', 5, []);
zlim([0 100]);
w = 50;
ylim([-w w]);
xlim([-w w]);
daspect([1 1 1]);
grid on;
box on;
colormap bone;
view(3);
view([-32, 30]);
xlabel('x (cm)');
ylabel('y (cm)');
zlabel('z (cm)');

set(gca, 'FontSize', 18, 'FontName', 'Times New Roman');


dx = -20;
dy = 15;
handles = DrawTipFrame([1 1 1].',1,[dx;dy;0],[]);
tightfig;
% 
% view([-60 30]);
% zoom(10);
% w = 10;
% ylim([-27.64-w -27.64+w]);
% xlim([-47.87-w -47.87+w]);
% zlim([76.79-w 76.79+w]);