function [map,nodes] = ShrinkMap(map,nodes, amount)
rot = [cos(pi/2) -sin(pi/2); sin(pi/2) cos(pi/2)];
for i=1:size(map,1)
    wallTangent = map(i,3:4)-map(i,1:2);
    wallTangent = wallTangent/norm(wallTangent);
    wallNormal = (rot*wallTangent.').';
    
    
    
    deltas1 = nodes-repmat(map(i,1:2),size(nodes,1),1);
    deltas2 = nodes-repmat(map(i,3:4),size(nodes,1),1);
    
    deltas1 = sqrt(sum(deltas1.^2,2));
    deltas2 = sqrt(sum(deltas2.^2,2));
    deltas1 = deltas1<1e-3;
    deltas2 = deltas2<1e-3;
    
    vals = CheckPointsBetweenLineSegments(nodes,repmat(map(i,:),size(nodes,1),1));
    
    map(i,1:2) = map(i,1:2)+wallNormal*amount;
    map(i,3:4) = map(i,3:4)+wallNormal*amount; 
    
    vals = deltas1|deltas2|vals;
    
    nodes(vals,:) = nodes(vals,:)+repmat(wallNormal,sum(vals),1)*amount;
    
end
end