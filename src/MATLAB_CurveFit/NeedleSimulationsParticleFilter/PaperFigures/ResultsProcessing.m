clear; clc; close all;

addpath('../ctests/');

baseDir = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\JournalResults\';

method = 'FullState';
data = rdir(strcat(baseDir,'**\',method,'EstimatedPos.dat'))

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
    
    estBasePath = strcat(currDir, method, 'Estimated');
    truthBasePath = strcat(currDir,method, 'GroundTruth');
    
    estPos = load(strcat(estBasePath, 'Pos.dat'));
    truePos = load(strcat(truthBasePath, 'Pos.dat'));
    
    estRs = loadOrientations(strcat(estBasePath, 'Rs.dat'));
    trueRs = loadOrientations(strcat(truthBasePath, 'Rs.dat'));
    
    startId = 40;
    endId = size(estPos,1)-40;
    
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
    
    figHandle = figure;
    titles = ['x', 'y', 'z'];
    for j=1:3
        subplot(1,3,j);
        plot(1:size(truePos,1), truePos(:,j),'r', 'LineWidth', 2);
        hold on;
        plot(1:size(estPos,1), estPos(:,j),'b--', 'LineWidth', 2);
        title(titles(j));
        if (j == 3)
            legend('Gold Standard', 'Estimator Output');
        end
        xlabel('Time Step','FontSize', 10, 'FontName', 'Times New Roman');
        ylabel(sprintf('%s Position (mm)', titles(j)),'FontSize', 10, 'FontName', 'Times New Roman');
        grid on;
        set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
        %export_fig -transparent AccuracyResults.pdf
    end
    set(figHandle, 'Position', [100 100 800 150]);
    
    results{resultBin} = vertcat(results{resultBin},mean(sqrt(sum(delta.^2,2))));
end


figHandle = figure;
hold on;
% positon error plots
for i=1:length(results)
    for j=1:length(results{i})
        scatter(speeds(i),results{i}(j),200, 'x','k','lineWidth',1.5);
    end
    scatter(speeds(i), mean(results{i}), 200, 'r', 'lineWidth', 1.5);
end
legend('Measurement', 'Mean');
xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
ylabel('RMS Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
grid on;
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
xlim([0 3.5]);
set(figHandle, 'Position', [100 100 240 150]);
%export_fig -transparent AccuracyResults.pdf
