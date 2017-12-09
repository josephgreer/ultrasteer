% Assumes angle \in [0, 2*pi)
function idxs = NodeAngleToIdx(nodes, angles, nAngles)
angleIdxs = round((nAngles-1)*angles/(2*pi))+1;
idxs = (nodes-1)*nAngles+angleIdxs;
idxs(nodes == -1) = -1;
end