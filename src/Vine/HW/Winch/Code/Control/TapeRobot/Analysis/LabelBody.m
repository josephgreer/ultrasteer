clear; clc; close all;

imMask = '/Volumes/C/Joey/ultrasteer/data/VineVideos/Jpegs/%05d.jpg';
baseIm = 64;

points = [];

imName = sprintf(imMask,44+baseIm);
imshow(imread(imName));
while(true)
    [x y] = ginput(1);
    points = vertcat(points, [x y]);
end