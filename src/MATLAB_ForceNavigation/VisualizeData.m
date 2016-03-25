clear; clc; close all;

basePath = 'C:\Users\CHARM\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\2_29_16\Trial100\';
im = imread(strcat(basePath, 'scan.png'));
im = im(80:535,111:552);
imshow(im);

force = load(strcat(basePath, 'force.mat'), '-ascii');
pos = load(strcat(basePath, 'pos.mat'), '-ascii');

% for i=1:size(force,1)
%     h = viscircles(pos(i,1:2), 5);
%     %pause(0.05);
% end
velocity = sqrt(sum((pos(2:end,4:end)-pos(1:end-1,4:end)).^2, 2))/5;

normForce = sqrt(sum(force(:,1:3).^2, 2));

[b,a] = butter(10,0.05,'high');
highPassForce = abs(filtfilt(b,a,normForce));

forceToPlot = highPassForce;
minForce = min(forceToPlot);
maxForce = max(forceToPlot);

hold on;
colors = [forceToPlot zeros(size(forceToPlot,1),2)];
minRep = [ones(size(forceToPlot,1),1)*minForce zeros(size(forceToPlot,1),2)];
colors = (colors-minRep)/maxForce*1;
scatter(pos(:,1), pos(:,2), 200, colors);

figure;
hold on;
plot(normForce,'r');
plot(filtfilt(b,a,normForce),'b');
plot(velocity, 'g');