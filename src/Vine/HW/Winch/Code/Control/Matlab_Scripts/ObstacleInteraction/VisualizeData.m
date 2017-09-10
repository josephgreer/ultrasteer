clear; clc; close all;

run = 2;

pts = load(sprintf('%02d.mat',run));
pts = pts.pts;

xs = []; ys = [];
scatter(pts{1}(:,1),pts{1}(:,2),'r');


hold on;
for i=2:length(pts)
    if(size(pts{i},1) == 0)
        continue;
    end
    xs = vertcat(xs,pts{i}(:,1));
    ys = vertcat(ys,pts{i}(:,2));
end

scatter(xs,ys,'b');
daspect([1 1 1]);