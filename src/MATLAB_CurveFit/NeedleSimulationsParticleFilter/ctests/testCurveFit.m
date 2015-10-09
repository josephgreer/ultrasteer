clear; clc; close all;

polyX = [-7e-3 8e-3 1e-1 0.1];
polyY = [5e-3 1e-3 -3e-1 0.1];
polyZ = [0 0 0 1];

zrange = [-10 10];

npoints = 500;

zpt = rand(npoints,1)*20-10;
xpt = polyval(polyX, zpt);
ypt = polyval(polyY, zpt);

noise = mvnrnd(zeros(3,1), diag([1 1 1]), npoints);

scatter3(xpt+noise(:,1), ypt+noise(:,2), zpt+noise(:,3));

save('./data/xpt.dat', 'xpt', '-ascii');
save('./data/xpt.dat', 'xpt', '-ascii');
save('./data/xpt.dat', 'xpt', '-ascii');
xlabel('x'); ylabel('y'); zlabel('z');
daspect([1 1 1]);