function [nodes,angles] = IndexToNodeAngle(idxs, nAngles, thetas)
nodes = floor(idxs/nAngles)+1;
angles = idxs-(nodes-1)*(nAngles); 
angles = thetas(angles);
end