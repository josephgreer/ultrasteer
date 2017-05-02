clear; clc; close all;
load 1_asymmetric.mat

nstates = params.nGridPoints^2*params.nRadPoints;
[XX, YY, rr] = GenerateGrid(params);

flatX = XX(:); flatY = YY(:); flatR = rr(:);


xpts = linspace(0,params.gridWidth,params.nGridPoints).';
ypts = linspace(0,params.gridHeight,params.nGridPoints).';

xrange = [3:params.nGridPoints-2];
yrange = [10:params.nGridPoints-2];
Psucc = reshape(Psucc, params.nGridPoints, params.nGridPoints);
imagesc(xpts(xrange),ypts(yrange),Psucc(yrange,xrange));
figure;
surf(xpts(xrange),ypts(yrange),Psucc(yrange,xrange));
xlabel('x (m)');
ylabel('y (m)');
zlabel('P(success)');
axis equal;
zlim([0 1.25]);
daspect([1 1 0.1]);
box on;
export_fig -transparent 1_asymmetric.pdf