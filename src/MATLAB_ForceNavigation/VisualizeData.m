clear; clc; close all;

basePath = 'C:\Users\CHARM\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\2_29_16\Trial0\';
im = imread(strcat(basePath, 'scan.png'));
imshow(im);

force = load(strcat(basePath, 'force.mat'), '-ascii');
pos = load(strcat(basePath, 'pos.mat'), '-ascii');

for i=1:size(force,1)
    h = viscircles(pos(i,1:2), 5);
    pause(0.05);
    delete(h);
end

