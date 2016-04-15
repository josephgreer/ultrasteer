clear; clc; close all;

path = 'C:\Users\CHARM\Desktop\Giada.mat';
TipHistory = load(path, '-ascii');

scatter3(TipHistory(:,1), TipHistory(:,2), TipHistory(:,3));
daspect([1 1 1]);

[r c circ_points] = findcurvature(TipHistory, 100);

hold on;
plot3(circ_points(:,1), circ_points(:,2), circ_points(:,3))