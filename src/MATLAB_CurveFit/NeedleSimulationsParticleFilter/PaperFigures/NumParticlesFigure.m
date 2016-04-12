clear; clc; close all;

methods = {'FullState','Marginalized'};

colors = {'r','b'};
col = {[1 0 0], [0 0 1]};

figHandle = figure(1);
subplot(1,3,1);
subplot(1,3,2);
subplot(1,3,3);
hold on;

for mth=1:length(methods)
    method = methods{mth};
    %baseDir = strcat('C:/Users/CHARM/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/NeedleScan/JournalResultsNumParticles/');
    baseDir = strcat('/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/NeedleScan/JournalResultsNumParticles/');
    data = rdir(strcat(baseDir,'**/Trial1/*',method,'EstimatedPos.dat'));
    
    nParticles = [];
    errors = [];
    for i=1:length(data)
        [preamble, ~] = regexp(data(i).name, strcat('.*/(\d+)', method, 'EstimatedPos\.dat'), 'tokens', 'match');
        numParticles = str2num(char(preamble{1}));
        
        % extract directory
        currDir = data(i).name;
        idxs = strfind(currDir, '/');
        currDir = currDir(1:idxs(end));
        
        estBasePath = strcat(currDir, num2str(numParticles), method, 'Estimated');
        truthBasePath = strcat(currDir,num2str(numParticles), method, 'GroundTruth');
        
        estPos = load(strcat(estBasePath, 'Pos.dat'));
        truePos = load(strcat(truthBasePath, 'Pos.dat'));
        
        [indicesR, indicesC] = ind2sub(size(estPos), find(estPos ~= 0));
        startId = min(indicesR);
        endId = size(estPos,1)-75;
        
        estPos = estPos([startId:endId], :);
        truePos = truePos([startId:endId], :);
        
        delta = estPos-truePos;
        nParticles = [nParticles numParticles];
        errors = [errors mean(sqrt(sum(delta.^2,2)))];
    end
    
    [~, sortis] = sort(nParticles);
    nParticles = nParticles(sortis);
    errors = errors(sortis);
    plot(nParticles, errors,colors{mth},'LineWidth',2);
    scatter(nParticles, errors,'MarkerFaceColor',col{mth},'MarkerEdgeColor','none');%,'MarkerEdgeColor',col{mth}*0.1,'LineWidth',1.5,'MarkerFaceColor',col{mth});
end
xlim([min(nParticles)-6 max(nParticles)+6]);
xlabel('Number of Particles','FontSize', 10, 'FontName', 'Times New Roman');
ylabel('Mean Error (mm)','FontSize', 10, 'FontName', 'Times New Roman');
title('Number of Particles vs. Accuracy');
grid on;
box on;
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman');
box on;
legend('Full State Particle Filter', 'Marginalized Particle Filter');%, 'Location', 'north');
set(figHandle, 'Position', [100 100 750 150]);
pause(1);
tightfig;
export_fig -transparent NumParticles.pdf