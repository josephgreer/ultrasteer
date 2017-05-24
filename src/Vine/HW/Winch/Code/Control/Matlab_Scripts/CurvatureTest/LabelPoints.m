clear; clc; close all;
directory = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/CurvatureTest/'; 
path = strcat(directory,'0_125.jpg');

image = imread(path);
imshow(image);


points = [];
while(true)
    [x y] = ginput(1);
    points = vertcat(points, [x y]);
end