function ignoreWalls = NodeIndexToIgnoreWalls(index,nodeTypes,indexToWalls)
ignoreWalls = -1;
if(nodeTypes(index) == 1)
    ignoreWalls = indexToWalls{index}
end
end