clear; clc; close all;

pointPath = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/PegForest/6/points.txt';

points = load(pointPath);

transitionPoints = find(points(:,1) == -1);
transitionPoints = [1;transitionPoints];

segments = [transitionPoints(1:end-1) transitionPoints(2:end)];
badSegs = find(segments(:,1) == segments(:,2)-1);
segments(badSegs,:) = []; 

figure;
hold on;

colormap jet;
map = colormap;
color_steps = size(map,1);

minTheta = 1e6;
maxTheta = -1e6;

centerY = 190.5;
circleDiameter = 219;

for i=1:size(segments,1)
    cPoints = points(segments(i,1):segments(i,2)-1,1:2);
    cPoints(find(cPoints(:,1) == -1 | cPoints(:,1) == 0),:) = [];
    
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
    initTheta = rad2deg(atan2(initDelta(2),initDelta(1)));
    
    if(initTheta<minTheta)
        minTheta = initTheta;
    end
    
    if(initTheta > maxTheta)
        maxTheta = initTheta;
    end
end

endThetas = [];

for i=1:size(segments,1)
    cPoints = points(segments(i,1):segments(i,2)-1,1:2);
    cPoints(find(cPoints(:,1) == -1 | cPoints(:,1) == 0),:) = [];
    
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
    initTheta = rad2deg(atan2(initDelta(2),initDelta(1)));
    
    lastGood = max(find((cPoints(end,1)-cPoints(:,1)) > 20));
    lastDelta = cPoints(end,:)-cPoints(lastGood,:);
    lastTheta = rad2deg(atan2(lastDelta(2),lastDelta(1)));
    
    endThetas = vertcat(endThetas, lastTheta);
        
    
    index = (initTheta-minTheta)/(maxTheta-minTheta);
    [~, minIdx] = min(abs(index*color_steps-linspace(1,color_steps,color_steps)));
    
    plot(cPoints(:,2),cPoints(:,1),'Color',map(minIdx,:),'LineWidth',5);
    
end

% ylim([325 650]);
% xlim([0 340]);
% xlim([400 600]);
box on;
grid on;
daspect([1 1 1]);
colorbar;
caxis([minTheta maxTheta]);

figure;
hist(endThetas)
std(endThetas(find(endThetas > 0)))
std(endThetas(find(endThetas < 0)))

mean(endThetas(find(endThetas > 0)))
mean(endThetas(find(endThetas < 0)))