% order is most proximal to distal
% thetas = [theta_(1); ...; theta_(n)] \in R^{n}
% ls = [l_1;...;l_n] \in R^{n}
function handles = DrawRobot(thetas, ls, p0, len, handles)
runThetas = cumsum(thetas);

rls = cumsum(ls);


if(len > 0)
    maxIdx = find(rls < len);
    if(isempty(maxIdx))
        fl = len;
        ls = [fl];
        runThetas = runThetas(1);
    else
        maxIdx = max(maxIdx);
        fl = len-rls(maxIdx);
        ls = [ls(1:maxIdx);fl];
        runThetas = runThetas(1:maxIdx+1);
    end
end

deltas = [cos(runThetas) sin(runThetas)].*repmat(ls(1:end),1,2);

ps = vertcat(p0.', deltas);
ps = cumsum(ps, 1);

if(isempty(handles.robot))
    handles.robot = plot(ps(:,1), ps(:,2),'LineWidth',1);
else
    set(handles.robot, 'XData', ps(:,1), 'YData', ps(:,2));
end
end