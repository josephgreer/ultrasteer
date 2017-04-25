% destination = [x,y] location to go to
function P = GenerateTransitionMatrix(destination,params)
nstates = params.nGridPoints^2*params.nRadPoints;

P = sparse(nstates,nstates);

[XX, YY, rr] = GenerateGrid(params);

XXd = repmat(destination(1),size(XX));
YYd = repmat(destination(2),size(YY));
flatXXd = XXd(:); flatYYd = YYd(:);

flatX = XX(:); flatY = YY(:); flatR = rr(:);

dX = flatXXd-flatX; dY = flatYYd-flatY;
desGridAngles = atan2(dY,dX);
deltaGridAngles = desGridAngles-flatR;
commands = sign(deltaGridAngles);
commands(find(abs(deltaGridAngles) < params.deadBandRad/2)) = 0;
swapCommands = find(abs(deltaGridAngles)>pi);
commands(swapCommands) = commands(swapCommands)*-1;

leftTurnOrientations = flatR-params.radStep;
straightOrientations = flatR;
rightTurnOrientations = flatR+params.radStep;

destinationLeftTurn = horzcat(params.actuatorSpacing*[cos(leftTurnOrientations) sin(leftTurnOrientations)]+[flatX,flatY], leftTurnOrientations);
destinationStraight = horzcat(params.actuatorSpacing*[cos(straightOrientations) sin(straightOrientations)]+[flatX,flatY], straightOrientations);
destinationRightTurn = horzcat(params.actuatorSpacing*[cos(rightTurnOrientations) sin(rightTurnOrientations)]+[flatX,flatY], rightTurnOrientations);

destinationLeftTurn = StateToLinear(params, destinationLeftTurn);
destinationStraight = StateToLinear(params, destinationStraight);
destinationRightTurn = StateToLinear(params, destinationRightTurn);

% P(sub2ind(size(P),linspace(1,nstates,nstates).',destinationLeftTurn)) = params.Pturns(1,(commands+2)).';
% P(sub2ind(size(P),linspace(1,nstates,nstates).',destinationStraight)) = params.Pturns(2,(commands+2)).';
% P(sub2ind(size(P),linspace(1,nstates,nstates).',destinationRightTurn)) = params.Pturns(3,(commands+2)).';

for i=1:nstates
    P(i,destinationLeftTurn(i)) = P(i,destinationLeftTurn(i))+params.Pturns(1,commands(i)+2);
    P(i,destinationStraight(i)) = P(i,destinationStraight(i))+params.Pturns(2,commands(i)+2);
    P(i,destinationRightTurn(i)) = P(i,destinationRightTurn(i))+params.Pturns(3,commands(i)+2);
end

destinationIdxs = find(flatX == destination(1) & flatY == destination(2));
for i=1:length(destinationIdxs)
    idx = destinationIdxs(i);
    P(idx,destinationLeftTurn(idx)) = 0;
    P(idx,destinationStraight(idx)) = 0;
    P(idx,destinationRightTurn(idx)) = 0;
    P(idx,idx) = 1;
end

end