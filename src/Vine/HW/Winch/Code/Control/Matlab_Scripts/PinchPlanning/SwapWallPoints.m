function map = SwapWallPoints(wallIndices, map)
temp = map(wallIndices,1:2);
map(wallIndices,1:2) = map(wallIndices,3:4);
map(wallIndices,3:4) = temp;
end