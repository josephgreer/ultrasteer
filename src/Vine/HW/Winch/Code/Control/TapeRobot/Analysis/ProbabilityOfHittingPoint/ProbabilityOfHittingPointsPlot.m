clear; clc; close all;

params = initParams();

[gridX, gridY] = GenerateGrid(params);
    
nstates = params.nGridPoints^2*params.nRadPoints;
[XX, YY, rr] = GenerateGrid(params);

flatX = XX(:); flatY = YY(:); flatR = rr(:);
startIdx = StateToLinear(params, [params.gridWidth/2, 0, pi/2]);
P0 = sparse(1,nstates);
P0(startIdx) = 1;

Psucc = zeros(params.nGridPoints^2,1);

xs = gridX(1:params.nGridPoints^2).';
ys = gridY(1:params.nGridPoints^2).';

goodIdxs = find(2.5 < ys);

ii = 1;
for i=goodIdxs.'
    display(sprintf('ii = %d of %d', ii, length(goodIdxs)));
    
    [destX, destY, ~] = findClosestPoint(params, [xs(i), ys(i), 0]);
    destination = [destX destY];
    
    P = GenerateTransitionMatrix(destination, params);
    
    Pend = P0;
    for jj=1:params.nActuators
        Pend = Pend*P;
    end
    
    Pend = reshape(Pend,params.nGridPoints^2,params.nRadPoints);
    Pend = sum(Pend,2);
    
    dists = [xs ys]-repmat([destX, destY], params.nGridPoints^2, 1);
    dists = sqrt(sum(dists.^2,2));
    
    goodPoints = find(dists <= params.successRad);
    Psucc(i) = sum(Pend(goodPoints));
%     maxDist = max(dists);
%     Psucc(i) = sum(((maxDist-dists)/maxDist).*Pend);
    
    
    ii = ii+1;
%     destLinear = StateToLinear(params, [destX, destY, 0]);
%     
%     Psucc(i) = Pend./(dists+0.1);
end

xpts = linspace(0,params.gridWidth,params.nGridPoints).';
ypts = linspace(0,params.gridHeight,params.nGridPoints).';
imagesc(xpts,ypts,reshape(Psucc,params.nGridPoints,params.nGridPoints));
xlabel('x');
ylabel('y');