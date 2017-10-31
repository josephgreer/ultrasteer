% order is most proximal to most distal
% x = [p_Nx; p_Ny ... p_1x; p_1y] \in R^{2*N}
function handles = DrawRobotXs(xs, len, handles)
deltas = xs(2:end,:)-xs(1:end-1,:);

ls = sqrt(sum(deltas.^2,2));
rls = cumsum(ls);

maxIdx = find(rls < len);
if(isempty(maxIdx))
    xx = [xs(1,:); xs(1,:) + len*deltas(1,:)/ls(1)];
else
    maxIdx = max(maxIdx);
    fl = len-rls(maxIdx);
    xx = [xs(1:maxIdx+1,:); xs(maxIdx+1,:)+fl*deltas(maxIdx+1,:)/ls(maxIdx+1)];
end

if(isempty(handles.robot))
    handles.robot = plot(xx(:,1), xx(:,2),'LineWidth',5);
else
    set(handles.robot, 'XData', xx(:,1), 'YData', xx(:,2));
end
end