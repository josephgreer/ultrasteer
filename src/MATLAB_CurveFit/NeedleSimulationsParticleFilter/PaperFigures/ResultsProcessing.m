clear; clc; 

addpath('../ctests/');

type = 'JournalResults';
baseDir = strcat('C:\Users\CHARM\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\', type, '\');

method = 'FullState';
data = rdir(strcat(baseDir,'**\',method,'EstimatedPos.dat'))

speedLabels = {'05mm', '1mm', '2mm', '3mm'};
speeds = [0.5 1 2 3];

showPlots = 0;

results = {[],[],[],[]};
resultsAxial = {[], [], [], []};
resultsLateral = {[], [], [], []};
for i=1:length(data)
    display(data(i).name)
    %close all;
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
    
    [indicesR, indicesC] = ind2sub(size(estPos), find(estPos ~= 0));
    startId = min(indicesR);
    endId = size(estPos,1)-20;
    
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
    
%     figHandle = figure;
%     titles = ['x', 'y', 'z'];
%     for j=1:3
%         subplot(1,3,j);
%         plot(1:size(truePos,1), truePos(:,j),'r', 'LineWidth', 2);
%         hold on;
%         plot(1:size(estPos,1), estPos(:,j),'b--', 'LineWidth', 2);
%         title(titles(j));
%         if (j == 3)
%             legend('Gold Standard', 'Estimator Output');
%         end
%         xlabel('Time Step','FontSize', 10, 'FontName', 'Times New Roman');
%         ylabel(sprintf('%s Position (mm)', titles(j)),'FontSize', 10, 'FontName', 'Times New Roman');
%         grid on;
%         set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
%         %export_fig -transparent AccuracyResults.pdf
%     end
%     set(figHandle, 'Position', [100 100 800 150]);
    
    display(mean(sqrt(sum(delta.^2,2))));
    results{resultBin} = vertcat(results{resultBin},mean(sqrt(sum(delta.^2,2))));
    
    
    titles = ['x', 'y', 'z'];
    deltaZs = zeros(size(truePos));
    for i=1:length(trueRs)
        deltaZs(i,:) = (inv(trueRs{i})*(truePos(i,:)-estPos(i,:))')';
    end
    resultsAxial{resultBin} = vertcat(resultsAxial{resultBin},mean(sqrt(sum(deltaZs(:,3).^2,2))));
    resultsLateral{resultBin} = vertcat(resultsLateral{resultBin},mean(sqrt(sum(deltaZs(:,1:2).^2,2))));
end


figHandle = figure;
subplot(1,3,1);
hold on;
% positon error plots
for i=1:length(results)
    for j=1:length(results{i})
        scatter(speeds(i),results{i}(j),100, 'MarkerEdgeColor',[0.2 0.2 0.2],'MarkerFaceColor',[0.5 0.5 0.5],'LineWidth',1.5)
    end
    scatter(speeds(i), mean(results{i}), 100, 'MarkerEdgeColor',[0.3 0 0],'MarkerFaceColor',[0.7 0 0],'LineWidth',1.5)
end
legend('Measurement', 'Mean');
xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
ylabel('RMS Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
grid on;
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
title(strcat(type, method));
xlim([0 3.5]);
ylim([0 10]);

subplot(1,3,2);
hold on;
% positon error plots
for i=1:length(resultsAxial)
    for j=1:length(resultsAxial{i})
        scatter(speeds(i),resultsAxial{i}(j),100, 'MarkerEdgeColor',[0.2 0.2 0.2],'MarkerFaceColor',[0.5 0.5 0.5],'LineWidth',1.5)
    end
    scatter(speeds(i), mean(resultsAxial{i}), 100, 'MarkerEdgeColor',[0.3 0 0],'MarkerFaceColor',[0.7 0 0],'LineWidth',1.5)
end
legend('Measurement', 'Mean');
xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
ylabel('Axial Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
grid on;
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
title(strcat(type, method));
xlim([0 3.5]);
ylim([0 10]);
%export_fig -transparent AccuracyResults.pdf


subplot(1,3,3);
hold on;
% positon error plots
for i=1:length(resultsLateral)
    for j=1:length(resultsLateral{i})
        scatter(speeds(i),resultsLateral{i}(j),100, 'MarkerEdgeColor',[0.2 0.2 0.2],'MarkerFaceColor',[0.5 0.5 0.5],'LineWidth',1.5)
    end
    scatter(speeds(i), mean(resultsLateral{i}), 100, 'MarkerEdgeColor',[0.3 0 0],'MarkerFaceColor',[0.7 0 0],'LineWidth',1.5)
end
legend('Measurement', 'Mean');
xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
ylabel('Lateral Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
grid on;
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
title(strcat(type, method));
xlim([0 3.5]);
ylim([0 10]);
set(figHandle, 'Position', [100 100 800 240]);
tightfig
