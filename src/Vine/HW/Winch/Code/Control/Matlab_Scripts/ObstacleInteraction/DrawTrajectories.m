clear; clc; close all;

pointPath = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/PegForest/4/points.txt';

points = load(pointPath);

transitionPoints = find(points(:,1) == -1);
transitionPoints = [1;transitionPoints];

segments = [transitionPoints(1:end-1) transitionPoints(2:end)];
badSegs = find(segments(:,1) == segments(:,2)-1);
segments(badSegs,:) = []; 


wall_axis = [575-570; 425-1];
wall_axis = wall_axis/norm(wall_axis);

h = figure;
hold on;

% colormap jet;
map = colormap;
color_steps = size(map,1);

minTheta = 90;
maxTheta = -90;

for i=1:size(segments,1)
    cPoints = points(segments(i,1):segments(i,2)-1,1:2);
    cPoints(find(cPoints(:,1) == -1),:) = [];
    
    changed = true;
    while(changed)
        badPoints = find(cPoints(2:end,1) - cPoints(1:end-1,1) < -2);
        cPoints(badPoints+1,:) = [];
        if(length(badPoints) == 0)
            changed = false;
        end
    end
%     plot(cPoints(:,1),cPoints(:,2));

    firstGood = min(find((cPoints(:,1)-cPoints(1,1)) > 20));

    initDelta = cPoints(firstGood,:)-cPoints(1,:);
    initDelta = initDelta/norm(initDelta);
    initTheta = 180-rad2deg(acos(initDelta*wall_axis))-90;
    
    if(initTheta<minTheta)
        minTheta = initTheta;
    end
    
    if(initTheta > maxTheta)
        maxTheta = initTheta;
    end
end

for i=1:size(segments,1)
    cPoints = points(segments(i,1):segments(i,2)-1,1:2);
    cPoints(find(cPoints(:,1) == -1),:) = [];
    
    changed = true;
    while(changed)
        badPoints = find(cPoints(2:end,1) - cPoints(1:end-1,1) < -2);
        cPoints(badPoints+1,:) = [];
        if(length(badPoints) == 0)
            changed = false;
        end
    end
%     plot(cPoints(:,1),cPoints(:,2));

    firstGood = min(find((cPoints(:,1)-cPoints(1,1)) > 20));

    initDelta = cPoints(firstGood,:)-cPoints(1,:);
    initDelta = initDelta/norm(initDelta);
    initTheta = 180-rad2deg(acos(initDelta*wall_axis))-90;
    
    cPoints(:,2) = cPoints(:,2)-cPoints(1,2);
    
    cPoints = cPoints*0.1275;
        
    
    index = (initTheta-minTheta)/(maxTheta-minTheta);
    [~, minIdx] = min(abs(index*color_steps-linspace(1,color_steps,color_steps)));
    
    plot(cPoints(:,2),cPoints(:,1)-50,'Color',map(minIdx,:),'LineWidth',5);
    
end

% xlim([-200 200]);
% ylim([400 600]);
% xlim([0 425]);
% xlim([-25, 600]);
% ylim([-200, 200]);

% ylim([400 600]);
% xlim([-200, 200]);
xlim([-30 30]);
ylim([0, 30]);


set(h, 'Position', [0 0 320, 220]);
box on;
grid on;
daspect([1 1 1]);
c = colorbar;
c.Label.String = 'Orientation, \theta (^o)';
caxis([minTheta maxTheta]);

box on;
grid on;

xlabel('x (cm)');
ylabel('y (cm)')

title({'Tip Trajectories of','Robot Interacting with Wall'});

set(gca,'FontSize',12,'FontName','Times New Roman');
% tightfig;

% tightfig;