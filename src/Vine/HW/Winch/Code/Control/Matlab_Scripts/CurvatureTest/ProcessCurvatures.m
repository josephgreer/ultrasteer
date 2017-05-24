clear; clc; close all;
directory = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/CurvatureTest/'; 
path = strcat(directory,'0_125.jpg');
pointsPath = strcat(directory, '0_125b.mat');

image = imread(path);
imshow(image);
hold on;

points = load(pointsPath);
points = points.points;


scatter(points(:,1), points(:,2));

pts = [points zeros(size(points,1),1)];
pts = pts(3:end,:);
[r, c, circ] = findcurvature(pts, 100);
scatter(circ(:,1), circ(:,2), 'r');

lengthScale = norm(points(1,:)-points(2,:))
lengthScale = 8.2e-2/lengthScale

r = r*lengthScale
curvature = 1/r