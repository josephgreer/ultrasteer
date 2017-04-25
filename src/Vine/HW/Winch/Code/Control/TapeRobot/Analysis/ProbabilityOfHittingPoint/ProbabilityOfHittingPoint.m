clear; clc; close all;

params = initParams();

[gridX, gridY] = GenerateGrid(params);

[destX, destY, ~] = findClosestPoint(params, [5, 5, 0]);
destination = [destX destY];

P = GenerateTransitionMatrix(destination, params);

nstates = params.nGridPoints^2*params.nRadPoints;
[XX, YY, rr] = GenerateGrid(params);

flatX = XX(:); flatY = YY(:); flatR = rr(:);
startIdx = StateToLinear(params, [params.gridWidth/2, 0, pi/2]);
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



% for i=1:size(P,1)
%     if(abs(sum(P(i,:)) - 1) > 0.05)
%         display(i);
%     end
% end