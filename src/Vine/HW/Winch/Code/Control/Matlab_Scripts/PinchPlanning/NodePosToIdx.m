function idx = NodePosToIdx(pos, nodes)
delts = repmat(pos,size(nodes,1),1)-nodes;
delts = sum(delts.^2,2);
[minVal,idx] = min(delts);
if(minVal > 1e-3)
    idx = -1;
end
end