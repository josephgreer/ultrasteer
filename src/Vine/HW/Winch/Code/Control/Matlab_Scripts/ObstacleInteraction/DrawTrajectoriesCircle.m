clear; clc; close all;

pointPath = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/PegForest/5/points.txt';

points = load(pointPath);

transitionPoints = find(points(:,1) == -1);
transitionPoints = [1;transitionPoints];

segments = [transitionPoints(1:end-1) transitionPoints(2:end)];
badSegs = find(segments(:,1) == segments(:,2)-1);
segments(badSegs,:) = []; 

h = figure;
hold on;

colormap jet;
map = colormap;
color_steps = size(map,1);

minY = -1;
maxY = 1;

centerY = 190.5;
circleDiameter = 219;

% for i=1:size(segments,1)
%     cPoints = points(segments(i,1):segments(i,2)-1,1:2);
%     cPoints(find(cPoints(:,1) == -1),:) = [];
%     
%     changed = true;
%     while(changed)
%         badPoints = find(cPoints(2:end,1) - cPoints(1:end-1,1) < -2);
%         cPoints(badPoints+1,:) = [];
%         if(length(badPoints) == 0)
%             changed = false;
%         end
%     end
%     
%     firstY = 2*(cPoints(1,2)-centerY)/circleDiameter;
%     
%     if(firstY < minY)
%         minY = firstY;
%     end
%     
%     if(firstY > maxY)
%         maxY = firstY;
%     end
% end


endThetas = [];

center = [400 200];
rot = deg2rad(4);

for i=1:size(segments,1)
    cPoints = points(segments(i,1):segments(i,2)-1,1:2);
    cPoints(find(cPoints(:,1) <= 0),:) = [];
    cPoints(find(cPoints(:,2) <= -120+200 & cPoints(:,1) >= 50+400 & cPoints(:,1) <= 100+400),:) = [];
    
    changed = true;
    while(changed)
        badPoints = find(cPoints(2:end,1) - cPoints(1:end-1,1) < -2);
        cPoints(badPoints+1,:) = [];
        if(length(badPoints) == 0)
            changed = false;
        end
    end
    firstY = 2*(cPoints(1,2)-centerY)/circleDiameter;
    
    cPoints = cPoints-repmat(center, size(cPoints,1), 1);
    cPoints = ([cos(rot) -sin(rot); sin(rot) cos(rot)]*cPoints.').';
    
    lastGood = max(find((cPoints(end,1)-cPoints(:,1)) > 20));
    lastDelta = cPoints(end,:)-cPoints(lastGood,:);
    lastTheta = rad2deg(atan2(lastDelta(2),lastDelta(1)));
    
    endThetas = vertcat(endThetas, lastTheta);
        
    
    index = (firstY-minY)/(maxY-minY);
    [~, minIdx] = min(abs(index*color_steps-linspace(1,color_steps,color_steps)));
    
    plot(cPoints(:,2)*0.1275,cPoints(:,1)*0.1275,'Color',map(minIdx,:),'LineWidth',5);
    
end

% ylim([325 650]);
% xlim([0 340]);
% xlim([400 600]);
box on;
grid on;
c = colorbar;
caxis([minY maxY]);
ylim([-50*0.1275 230*0.1275]);
xlim([-200*0.1275 200*0.1275]);
daspect([1 1 1]);
set(h,'Position',[0 0 320 200]);
set(gca,'FontSize',12,'FontName','Times New Roman');
title({'Tip Trajectories of Robot', 'Interacting with Circular Obstacle', 'with Varied Start Positions'});

c.Label.String = 'Horizontal Starting Pos. (x/D)';


figure;
hist(endThetas)
std(endThetas(find(endThetas > 0)))
std(endThetas(find(endThetas < 0)))

mean(endThetas(find(endThetas > 0)))
mean(endThetas(find(endThetas < 0)))