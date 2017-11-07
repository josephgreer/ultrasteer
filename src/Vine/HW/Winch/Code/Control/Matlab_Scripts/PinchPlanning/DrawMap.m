function DrawMap(map)
badIdx = find(map(:,1) == map(:,2) & map(:,2) == map(:,3) & map(:,3) == map(:,4) & map(:,1) == 0);
map(badIdx,:) = [];
for i=1:size(map,1)
    plot([map(i,1) map(i,3)], [map(i,2) map(i,4)],'k','LineWidth',2);
end
end