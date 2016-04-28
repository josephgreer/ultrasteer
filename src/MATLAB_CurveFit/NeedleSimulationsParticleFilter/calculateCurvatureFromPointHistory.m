clear; clc; close all;

path = 'C:\Giada\Data\4_26_16_Curvature\Trial8\PointHistory.mat';
TipHistory = load(path, '-ascii');

[~,idxs] = sort(TipHistory(:,1));
TipHistory = TipHistory(idxs,:);
TipHistory = TipHistory(2:end,:);

scatter3(TipHistory(:,1), TipHistory(:,2), TipHistory(:,3));
daspect([1 1 1]);

[r c circ_points] = findcurvature(TipHistory, 100);

hold on;
plot3(circ_points(:,1), circ_points(:,2), circ_points(:,3))