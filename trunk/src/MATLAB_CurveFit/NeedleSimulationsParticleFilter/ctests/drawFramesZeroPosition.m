%draw particle frames all with the zeroed positions
function handles = drawFramesZeroPosition(fig, xp, scale, handles, params)
for i=1:length(xp)
    xp{i}.pos = zeros(3,1);
end
handles = drawFrames(fig, xp, scale, params, handles);
end