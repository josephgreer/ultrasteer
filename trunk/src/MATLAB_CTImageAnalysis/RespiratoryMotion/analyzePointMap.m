clear; clc; close all;

load('diaphragm_lateral.mat');

pts = values(handles.ptMap);

figure(1);
hold on;
for i=1:length(pts)
    pts{i} = pts{i}.*handles.info.ImagerPixelSpacing';
    scatter(pts{i}(1), pts{i}(2));
end

maxDist = 0;
for i=1:length(pts)
    for j=i+1:length(pts)
        dist = norm(pts{i}-pts{j});
        maxDist = max(dist,maxDist);
    end
end

fprintf('Maximum distance in dataset %f\n', maxDist);