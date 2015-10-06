clear; clc; close all;

addpath('../ctests/');

baseDir = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\9_18_15\';

data = rdir(strcat(baseDir,'**\EstimatedPos.dat'))


speedLabels = {'05mm', '1mm', '2mm', '3mm'};
speeds = [0.5 1 2 3];

showPlots = 0;

results = {[],[],[],[]};
for i=1:length(data)
    close all;
    currDir = data(i).name;
    idxs = strfind(currDir, '\');
    currDir = currDir(1:idxs(end));
    
    speedLabel = currDir(idxs(end-2)+1:idxs(end-1)-1);
    
    resultBin = -1;
    for jj=1:length(speedLabels)
        if(strcmpi(speedLabel,speedLabels(jj)))
            resultBin = jj;
            break;
        end
    end
    
    estBasePath = strcat(currDir,'Estimated');
    truthBasePath = strcat(currDir,'GroundTruth');
    
    estPos = load(strcat(estBasePath, 'Pos.dat'));
    truePos = load(strcat(truthBasePath, 'Pos.dat'));
    
    estRs = loadOrientations(strcat(estBasePath, 'Rs.dat'));
    trueRs = loadOrientations(strcat(truthBasePath, 'Rs.dat'));
    
    startId = 20;
    endId = size(estPos,1)-1;
    
    estPos = estPos([startId:endId], :);
    truePos = truePos([startId:endId], :);
    
    estRs = estRs([startId:endId]);
    trueRs = trueRs([startId:endId]);
    
    if(showPlots)
        scatter3(estPos(:,1), estPos(:,2), estPos(:,3), 'b');
        hold on;
        scatter3(truePos(:,1), truePos(:,2), truePos(:,3), 'r');
        daspect([1 1 1]);
        xlabel('x');
        ylabel('y');
        zlabel('z');
    end
    
    delta = estPos-truePos;
    
    figure;
    titles = ['x', 'y', 'z'];
    for i=1:3
        subplot(2,2,i);
        plot(1:size(truePos,1), truePos(:,i),'b');
        hold on;
        plot(1:size(estPos,1), estPos(:,i),'r--');
        title(titles(i));
    end
    
    results{resultBin} = vertcat(results{resultBin},mean(sqrt(sum(delta.^2,2))));
end


figure;
hold on;
% positon error plots
for i=1:length(results)
    for j=1:length(results{i})
        scatter(speeds(i),results{i}(j),200, 'x','k','lineWidth',1.5);
    end
    scatter(speeds(i), mean(results{i}), 200, 'r', 'lineWidth', 1.5);
end
xlabel('Insertion Velocity (mm/s),','FontSize', 14, 'FontName', 'Times New Roman');
ylabel('RMS Error (mm)','FontSize', 14, 'FontName', 'Times New Roman');
grid on;
set(gca, 'FontSize', 14, 'FontName', 'Times New Roman');
xlim([0 3.5]);