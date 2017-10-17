clear; clc; close all;

path = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/Maze3/001.jpg';
im = imread(path);

imshow(im);

[x y] = ginput;