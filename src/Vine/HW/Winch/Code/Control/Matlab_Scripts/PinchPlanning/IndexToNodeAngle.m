function [nodes,angles] = IndexToNodeAngle(idxs, nAngles, thetas)
nodes = floor((idxs-1)/nAngles)+1;
angles = idxs-(nodes-1)*(nAngles); 
angles = thetas(angles);
end