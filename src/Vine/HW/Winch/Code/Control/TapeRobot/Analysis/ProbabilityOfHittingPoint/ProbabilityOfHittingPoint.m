clear; clc; close all;

params = initParams();

[gridX, gridY] = GenerateGrid(params);

[destX, destY, ~] = findClosestPoint(params, [4.48, 4.1, 0]);
% destX = gridX(size(gridX,1)/2,size(gridX,2)/2,1);
% destY = gridY(size(gridY,1)/2,size(gridY,2)/2,1);
destination = [destX destY];

P = GenerateTransitionMatrix(destination, params);

nstates = params.nGridPoints^2*params.nRadPoints;
[XX, YY, rr] = GenerateGrid(params);

flatX = XX(:); flatY = YY(:); flatR = rr(:);
startIdx = StateToLinear(params, [5, 0, pi/2]);
P0 = sparse(1,nstates);
P0(startIdx) = 1;

% Pend = P0*P^params.nActuators

Pend = P0;
for i=1:params.nActuators
    Pend = Pend*P;
end

Pend = reshape(Pend,params.nGridPoints^2,params.nRadPoints);
Pend = sum(Pend,2);

xpts = linspace(0,params.gridWidth,params.nGridPoints).';
ypts = linspace(0,params.gridHeight,params.nGridPoints).';
imagesc(xpts,ypts,reshape(Pend,params.nGridPoints,params.nGridPoints));
xlabel('x');
ylabel('y');