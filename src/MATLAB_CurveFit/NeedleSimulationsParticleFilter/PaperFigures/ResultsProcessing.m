clear; clc; close all;

addpath('../ctests/');
addpath('../');

methods = {'FullState', 'Marginalized'};
figHandle = figure(1);
figHandle2 = figure(2);

normalLineColor = [0 23/255 102/255;...
                   64/255 0 0;...
                   0.4 0.6 0.7];
               
meanLineColor = normalLineColor;               

normalFaceColor = [163 185 255;...
                 255 194 194;...
                 255 194 194]/255;
meanFaceColor = [0 0 255/255;...
                255/255 0 0;...
                0.4 0.6 0.7];
            
lineColors = {'r','k'};

output_lateral = 0;
types = {'JournalResults','JournalResultsNoDoppler','JournalResultsNaive'};

for mth=1:length(methods)
    for tp = 1:length(types)
        baseDir = strcat('C:/Users/CHARM/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/NeedleScan/', types{tp}, '/');
        %baseDir = strcat('/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/NeedleScan/', types{tp}, '/');
        
        method = methods{mth};
        data = rdir(strcat(baseDir,'**\',method,'EstimatedPos.dat'))
        
        speedLabels = {'05mm', '1mm', '2mm', '3mm'};
        speeds = [0.5 1 2 3];
        
        showPlots = 0;
        
        results = {[],[],[],[]};
        resultsAxial = {[], [], [], []};
        resultsLateral = {[], [], [], []};
        resultsOrientationError = {[], [], [], []};
        resultsMeasDistance = [];
        resultsMax = {[], [], [], []};
        resultsStd = {[], [], [], []};
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
            
            measPos = load(strcat(currDir,'MeasPos.dat'))';
            deltaMeasPos = truePos-measPos;
            
            estRs = loadOrientations(strcat(estBasePath, 'Rs.dat'));
            trueRs = loadOrientations(strcat(truthBasePath, 'Rs.dat'));
            
            [indicesR, indicesC] = ind2sub(size(estPos), find(estPos ~= 0));
            startId = min(indicesR);
            endId = size(estPos,1)-20;
            
            estPos = estPos([startId:endId], :);
            truePos = truePos([startId:endId], :);
            
            estRs = estRs([startId:endId]);
            trueRs = trueRs([startId:endId]);
            
            delta = estPos-truePos;
            resultsMeasDistance = vertcat(resultsMeasDistance,[mean(sqrt(sum(delta.^2,2))) mean(sqrt(sum(deltaMeasPos.^2,2)))]);
            
            if(showPlots)
                scatter3(estPos(:,1), estPos(:,2), estPos(:,3), 'b');
                hold on;
                scatter3(truePos(:,1), truePos(:,2), truePos(:,3), 'r');
                daspect([1 1 1]);
                xlabel('x');
                ylabel('y');
                zlabel('z');
            end
            
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
            
            %display(mean(sqrt(sum(delta.^2,2))));
            results{resultBin} = vertcat(results{resultBin},mean(sqrt(sum(delta.^2,2))));
            resultsMax{resultBin} = vertcat(resultsMax{resultBin}, max(sqrt(sum(delta.^2,2))));
            resultsStd{resultBin} = vertcat(resultsStd{resultBin}, std(sqrt(sum(delta.^2,2))));
            
            
            titles = ['x', 'y', 'z'];
            deltaZs = zeros(size(truePos));
            for i=1:length(trueRs)
                deltaZs(i,:) = (trueRs{i}'*(truePos(i,:)-estPos(i,:))')';
                if(isinf(deltaZs(i,:)))
                    xx = 0;
                end
            end
            
            orErrors = zeros(length(trueRs),1);
            for i=1:length(trueRs)
                orErrors(i) = SO3Metric(trueRs{i}, estRs{i});
            end
            
            resultsAxial{resultBin} = vertcat(resultsAxial{resultBin},mean(sqrt(sum(deltaZs(:,3).^2,2))));
            resultsLateral{resultBin} = vertcat(resultsLateral{resultBin},mean(sqrt(sum(deltaZs(:,1:2).^2,2))));
            resultsOrientationError{resultBin} = vertcat(resultsOrientationError{resultBin},mean(orErrors));
        end
        
        if(tp == 1)
            figure(1);
            pause(1);
            if(output_lateral)
                subplot(2,3,3*(mth-1)+1);
            else
                subplot(1,3,mth);
            end
            hold on;
            % positon error plots
            for i=1:length(results)
                for j=1:length(results{i})
                    scatter(speeds(i),results{i}(j),100, 'MarkerEdgeColor',normalLineColor(mth,:),'LineWidth',1.5);%'MarkerFaceColor',normalFaceColor(mth,:),'LineWidth',1.5)
                    %errorbar(speeds(i), results{i}(j), resultsStd{i}(j), 'rx');
                end
                scatter(speeds(i), mean(results{i}), 100, 'MarkerEdgeColor',meanLineColor(mth,:),'MarkerFaceColor',meanFaceColor(mth,:),'LineWidth',1.5)
                %errorbar(speeds(i), mean(results{i}), mean(resultsStd{i}), 'rx');
                
            end
            legend('Measurement', 'Mean', 'Location', 'northwest');
            xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
            ylabel('RMS Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
            grid on;
            box on;
            set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
            title(strcat(method, ' RMS Error'));
            xlim([0 3.5]);
            ylim([0 6]);
            
            figure(5);
            
            subplot(1,2,mth);
            hold on;
            % positon error plots
            for i=1:length(resultsMax)
                for j=1:length(resultsMax{i})
                    scatter(speeds(i),resultsMax{i}(j),100, 'MarkerEdgeColor',normalLineColor(mth,:),'LineWidth',1.5);%'MarkerFaceColor',normalFaceColor(mth,:),'LineWidth',1.5)
                end
                scatter(speeds(i), mean(resultsMax{i}), 100, 'MarkerEdgeColor',meanLineColor(mth,:),'MarkerFaceColor',meanFaceColor(mth,:),'LineWidth',1.5)
            end
            legend('Measurement', 'Mean', 'Location', 'northwest');
            xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
            ylabel('Max Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
            grid on;
            box on;
            set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
            title(strcat(method, ' RMS Error'));
            xlim([0 3.5]);
            ylim([0 6]);
            
            figure(6);
            subplot(1,2,mth);
            hold on;
            % positon error plots
            for i=1:length(resultsStd)
                for j=1:length(resultsStd{i})
                    scatter(speeds(i),resultsStd{i}(j),100, 'MarkerEdgeColor',normalLineColor(mth,:),'LineWidth',1.5);%'MarkerFaceColor',normalFaceColor(mth,:),'LineWidth',1.5)
                end
                scatter(speeds(i), mean(resultsStd{i}), 100, 'MarkerEdgeColor',meanLineColor(mth,:),'MarkerFaceColor',meanFaceColor(mth,:),'LineWidth',1.5)
            end
            legend('Measurement', 'Mean', 'Location', 'northwest');
            xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
            ylabel('Error Std (mm)','FontSize', 10, 'FontName', 'Times New Roman');
            grid on;
            box on;
            set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
            title(strcat(method, ' RMS Error'));
            xlim([0 3.5]);
            ylim([0 6]);
        end
        
        
        if(output_lateral && tp == 1)
            subplot(2,3,3*(mth-1)+2);
            hold on;
            % positon error plots
            
            means = [];
            for i=1:length(resultsLateral)
                for j=1:length(resultsLateral{i})
                    scatter(speeds(i),resultsLateral{i}(j),100, 'MarkerEdgeColor',normalLineColor(mth,:),'MarkerFaceColor',normalFaceColor(mth,:),'LineWidth',1.5)
                end
                scatter(speeds(i), mean(resultsLateral{i}), 100, 'MarkerEdgeColor',meanLineColor(mth,:),'MarkerFaceColor',meanFaceColor(mth,:),'LineWidth',1.5)
            end
            legend('Measurement', 'Mean', 'Location', 'northwest');
            xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
            ylabel('Lateral Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
            grid on;
            box on;
            set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
            title(strcat(method, ' Lateral Error'));
            xlim([0 3.5]);
            ylim([0 6]);
            
            subplot(2,3,3*(mth-1)+3);
            hold on;
            % positon error plots
            for i=1:length(resultsAxial)
                for j=1:length(resultsAxial{i})
                    scatter(speeds(i),resultsAxial{i}(j),100, 'MarkerEdgeColor',normalLineColor(mth,:),'MarkerFaceColor',normalFaceColor(mth,:),'LineWidth',1.5)
                end
                scatter(speeds(i), mean(resultsAxial{i}), 100, 'MarkerEdgeColor',meanLineColor(mth,:),'MarkerFaceColor',meanFaceColor(mth,:),'LineWidth',1.5)
            end
            legend('Measurement', 'Mean', 'Location', 'northwest');
            xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
            ylabel('Axial Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
            grid on;
            box on;
            set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
            title(strcat(method, ' Axial Error'));
            xlim([0 3.5]);
            ylim([0 6]);
        end
        
        if(strcmpi(methods{mth},'Marginalized') & tp ~= 3)
            figure(2);
            pause(1);
            subplot(1,3,1);
            hold on;
            
            % positon error plots
            title('RMS Error');
            %errorbar(speeds, cellfun(@mean, results), cellfun(@std, results),'MarkerEdgeColor',lineColors(tp,:));
            plot(speeds, cellfun(@mean, results),lineColors{tp},'LineWidth',2);
            legend('Doppler', 'No Doppler', 'Location', 'northwest');
            xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
            ylabel('RMS Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
            grid on;
            box on;
            set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
            xlim([0 3.5]);
            ylim([0 3]);
            
            subplot(1,3,2);
            title('Axial Error');
            hold on;
            %errorbar(speeds, cellfun(@mean, resultsAxial), cellfun(@std, resultsAxial),'MarkerEdgeColor',lineColors(tp,:));
            plot(speeds, cellfun(@mean, resultsAxial),lineColors{tp},'LineWidth',2);
            legend('Doppler', 'No Doppler', 'Location', 'northwest');
            xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
            ylabel('Axial Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
            grid on;
            box on;
            set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
            xlim([0 3.5]);
            ylim([0 3]);
            
            subplot(1,3,3);
            title('Lateral Error');
            hold on;
            %errorbar(speeds, cellfun(@mean, resultsLateral), cellfun(@std, resultsLateral),'MarkerEdgeColor',lineColors(tp,:));
            plot(speeds, cellfun(@mean, resultsLateral),lineColors{tp},'LineWidth',2);
            legend('Doppler', 'No Doppler', 'Location', 'northwest');
            xlabel('Insertion Velocity (mm/s),','FontSize', 10, 'FontName', 'Times New Roman');
            ylabel('Lateral Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
            grid on;
            box on;
            set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
            xlim([0 3.5]);
            ylim([0 3]);
            
            display(sprintf('marginalized %s, RMS %f Axial %f Lateral %f\n', types{tp}, mean(cellfun(@mean, results)), mean(cellfun(@mean, resultsAxial)),...
                mean(cellfun(@mean, resultsLateral))));
        end
        % measureDistance vs accuracy
        figure(3);
        if(tp ~= 3 || mth ~= 2)
            subplot(2, 2, tp*mth);
            scatter(resultsMeasDistance(:,2), resultsMeasDistance(:,1));
            title(strcat(methods{mth}, ' ', types{tp}));
            ylim([0.5 10]);
        end
        
%         display(sprintf('method %s type %s, Mean %f Std %f Max %f Axial %f Lateral %f\n', methods{mth}, types{tp}, mean(cellfun(@mean, results)),...
%             mean(cellfun(@mean, resultsStd)), mean(cellfun(@mean, resultsMax)), mean(cellfun(@mean, resultsAxial)),...
%             mean(cellfun(@mean, resultsLateral))));
    end
end
figure(1);
set(figHandle, 'Position', [100 100 750 150]);
subplot(1,3,3);
pause(1);
tightfig;
pause(1);

figure(5);
set(figHandle, 'Position', [100 100 750 200]);
pause(1);
tightfig;
pause(1);

figure(6);
set(figHandle, 'Position', [100 100 750 200]);
pause(1);
tightfig;
pause(1);
%export_fig -transparent AccuracyResults.pdf

% figure(4);
% set(figHandle, 'Position', [100 100 750 150]);
% subplot(1,3,3);
% pause(1);
% tightfig;
% pause(1);
% export_fig -transparent AccuracyResultsMax.pdf
% 
% figure(5);
% set(figHandle, 'Position', [100 100 750 150]);
% subplot(1,3,3);
% pause(1);
% tightfig;
% pause(1);
% export_fig -transparent AccuracyResultsStd.pdf

% figure(2);
% set(figHandle2, 'Position', [100 100 750 350]);
% pause(1);
% tightfig;
% pause(1);
% export_fig -transparent DopplerNoDopplerComparison.pdf
