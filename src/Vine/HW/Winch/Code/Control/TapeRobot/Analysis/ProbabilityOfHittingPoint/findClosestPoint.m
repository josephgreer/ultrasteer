% state = [x y r]
function [xx,yy,rr] = findClosestPoint(params, state)
state(:,1) = min(max(state(:,1),0),params.gridWidth);
state(:,2) = min(max(state(:,2),0), params.gridHeight);
state(:,3) = wrapTo2Pi(state(:,3));
rr = round(state(:,3)/params.radStep)*params.radStep;
xx = round(state(:,1)/params.widthStep)*params.widthStep;
yy = round(state(:,2)/params.heightStep)*params.heightStep;
end