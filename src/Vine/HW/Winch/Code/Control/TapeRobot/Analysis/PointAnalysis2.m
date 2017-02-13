clear; clc; close all;

points = load('points.txt');
points = points(1:end,:);

bodyPoints = load('bodyPoints.mat');
bodyPoints = bodyPoints.points;

obstaclePoints = load('obstaclePoints.mat');
obstaclePoints = obstaclePoints.points;

pos = points(:,1:2);
bodyPoints = bodyPoints-repmat(pos(1,:),size(bodyPoints,1),1);
obstaclePoints = obstaclePoints-repmat(pos(1,:),size(obstaclePoints,1),1);
pos = pos-repmat(pos(1,:),size(pos,1),1);
% pos = pos(1:600,:);
npoints = size(pos, 1);

d1 = designfilt('lowpassiir','FilterOrder',4, ...
    'HalfPowerFrequency',0.25,'DesignMethod','butter');
lowPassPos = zeros(size(pos));
lowPassPos(:,1) = filtfilt(d1,pos(:,1));
lowPassPos(:,2) = filtfilt(d1,pos(:,2));

figure;
scatter(lowPassPos(:,1),lowPassPos(:,2),'LineWidth',2);
% scatter(obstaclePoints(:,1), obstaclePoints(:,2),'k','LineWidth',2);
% hold on;
% scatter(bodyPoints(:,1),bodyPoints(:,2),'r','LineWidth',2);
% legend('head position');
grid on;
box on;
ylim([-200 100]);
xlim([0 550]);
daspect([1 1 1]);
axis ij;

export_fig -transparent 'headPoints.pdf';

