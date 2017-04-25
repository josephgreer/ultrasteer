% state = [x y r]
function idx = StateToLinear(params, state)
state(:,1) = min(max(state(:,1),0),params.gridWidth);
state(:,2) = min(max(state(:,2),0), params.gridHeight);
state(:,3) = wrapTo2Pi(state(:,3));
ridx = round(state(:,3)/params.radStep);
xidx = round(state(:,1)/params.widthStep);
yidx = round(state(:,2)/params.heightStep);

idx = ridx*params.nGridPoints^2+xidx*params.nGridPoints+yidx+1;
end