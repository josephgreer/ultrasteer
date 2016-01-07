% returns command from data history
function u = needleTestCommand(t, params, commands, dts)
ts = cumsum(dts);
% find the closest time in our list of commands
idx = min(find(ts >= t));
u = commands{idx};
end