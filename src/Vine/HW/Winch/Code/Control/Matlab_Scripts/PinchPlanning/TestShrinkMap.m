load('Maps/map');
load('Maps/nodes');

daspect([1 1 1]);
grid on;
hold on;
box on;
hold on;
xlabel('x (cm)');
ylabel('y (cm)');
set(gca,'FontSize',12,'FontName','Times New Roman');

DrawMap(map);

[map2,nodes2] = ShrinkMap(map,nodes,100);
DrawMap(map2);
scatter(nodes2(:,1),nodes2(:,2));