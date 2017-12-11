function [nodes,angles] = IndexToNodeAngle(idxs, nAngles, nNodes, thetas)
nodes = floor((idxs-1)/nAngles)+1;
angles = idxs-(nodes-1)*(nAngles); 
angles = thetas(angles);

% in the special case that our node is larger than nNodes this means that
% the node is an angle locked in verison of the node. This makes it so that
% we can't change angles twice.
nodes(nodes > nNodes) = nodes(nodes > nNodes) - nNodes;
end