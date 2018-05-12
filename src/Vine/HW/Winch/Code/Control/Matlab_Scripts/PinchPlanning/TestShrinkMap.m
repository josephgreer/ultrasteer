clear; close all;

load('Maps/map2');
load('Maps/nodes2');

daspect([1 1 1]);
grid on;
hold on;
box on;
hold on;
xlabel('x (cm)');
ylabel('y (cm)');
set(gca,'FontSize',12,'FontName','Times New Roman');

DrawMap(map);
scatter(nodes(:,1),nodes(:,2));

[map,nodes] = ShrinkMap(map,nodes,0.95*8*90/8);

DrawMap(map);

figure;
daspect([1 1 1]);
grid on;
hold on;
box on;
hold on;
xlabel('x (cm)');
ylabel('y (cm)');
set(gca,'FontSize',12,'FontName','Times New Roman');
DrawMap(map);
scatter(nodes(:,1),nodes(:,2));