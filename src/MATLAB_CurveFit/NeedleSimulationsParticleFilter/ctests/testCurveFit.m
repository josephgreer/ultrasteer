clear; clc; close all;

addpath('../');

polyX = [-7e-3 8e-3 1e-1 0.1];
polyY = [5e-3 1e-3 -3e-1 0.1];
polyZ = [0 0 1 0];

zrange = [-10 10];

npoints = 500;

zpt = rand(npoints,1)*20-10;
xpt = polyval(polyX, zpt);
ypt = polyval(polyY, zpt);

noise = mvnrnd(zeros(3,1), diag([1 1 1]), npoints);

scatter3(xpt+noise(:,1), ypt+noise(:,2), zpt+noise(:,3));

save('./data/xpt.dat', 'xpt', '-ascii');
save('./data/ypt.dat', 'ypt', '-ascii');
save('./data/zpt.dat', 'zpt', '-ascii');
xlabel('x'); ylabel('y'); zlabel('z');
daspect([1 1 1]);

polyval(polyX, 1)
polyval(polyY, 1)

polyval(polyX, -1)
polyval(polyY, -1)

polyval(polyX, 5e-3)
polyval(polyY, 5e-3)

load('./data/uniform_pts.dat');
hold on;
scatter3(uniform_pts(:,1), uniform_pts(:,2), uniform_pts(:,3), 'r');

p = Poly3([xpt ypt zpt], 3, 3);
eval(p, 1)
eval(p, -1)
eval(p, 5e-3)

pps = uniformlySpacedPoints(p, 2, 3);
delta = pps(1:end-1,:)-pps(2:end,:);
sqrt(sum(delta.^2,2))
scatter3(pps(:,1),pps(:,2),pps(:,3),'k')

opt1 = eval(p, 5)+10*ones(1,3);
opt2 = eval(p,p.range(2)+3)+10*ones(1,3);

ppts = [xpt ypt zpt];
ppts = [ppts; opt1; opt2];

[inliers, outliers] = findInlierPts(p, ppts, 3)