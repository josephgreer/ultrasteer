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

topLeftNode = [1365.50000000000,1656.00000000000];
topRightNode = [2079.50000000000,1656.00000000000];

for i=1:size(map,1)
    if(norm(map(i,1:2)-topLeftNode) < 1e-3 || norm(map(i,1:2)- topRightNode) < 1e-3)
        map(i,2) = map(i,2)+1.5*90;
    end
    if(norm(map(i,3:4)-topLeftNode) < 1e-3 || norm(map(i,3:4)- topRightNode) < 1e-3)
        map(i,4) = map(i,4)+1.5*90;
    end
end

for i=1:size(nodes,1)
    if(norm(nodes(i,:)-topLeftNode) < 1e-3 || norm(nodes(i,:)-topRightNode) < 1e-3)
        nodes(i,2) = nodes(i,2)+1.5*90;
    end
end

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