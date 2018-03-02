function cmap = PerturbMap(map, noise)
cmap = map;
for i=1:size(map,1)
    for j=1:2
        currNode = map(i,2*(j-1)+1:2*(j-1)+2);
        deltas1 = map(:,1:2)-repmat(currNode,size(map,1),1);
        deltas2 = map(:,3:4)-repmat(currNode,size(map,1),1);
        deltas1 = sqrt(sum(deltas1.^2,2));
        deltas2 = sqrt(sum(deltas2.^2,2));
        
        perturbedNode = currNode+normrnd(0,noise,1,2);
        cmap(deltas1 < 1e-3, 1:2) = perturbedNode;
        cmap(deltas2 < 1e-3, 3:4) = perturbedNode;
    end
end
end