clearvars -except 'truePosB' 'estPosB'; clc; close all;

method = 'FullState';
types = {'resultsOpenLoop/FullState', 'results0_5FullState/FullState', 'resultsOrig/Marginalized'};
nTypes = length(types);

addpath('../ctests');
addpath('..');

showGraphs = 0;

figHandle = figure;

mins = ones(3,1)*1e6;
maxs = ones(3,1)*-1e6;
startId = 115;
skip = 4;
skipR = 20;

estAxisColor = [0 0 255]/255;
trueAxisColor = [0 1 0];
for tp=1:length(types)
    subplot(1,nTypes,tp);
    estBasePath = strcat('data/trajectories/', types{tp}, 'Estimated');
    truthBasePath = strcat('data/trajectories/', types{tp}, 'GroundTruth');
    estPos = load(strcat(estBasePath, 'Pos.dat'));
    truePos = load(strcat(truthBasePath, 'Pos.dat'));
    
    estRs = loadOrientations(strcat(estBasePath, 'Rs.dat'));
    trueRs = loadOrientations(strcat(truthBasePath, 'Rs.dat'));
    
    [indicesR, indicesC] = ind2sub(size(estPos), find(estPos ~= 0));
    %startId = min(indicesR);
    endId = size(estPos,1)-20;
    idxs = [startId:skip:endId];
    idxsR = [1:skipR:length(idxs)];
    
    estPos = estPos(idxs, :);
    truePos = truePos(idxs, :);
    
    estRs = estRs(idxs);
    trueRs = trueRs(idxs);
    
    plot3(estPos(:,1), estPos(:,2), estPos(:,3),'r','LineWidth',2);
    hold on;
    plot3(truePos(:,1), truePos(:,2), truePos(:,3),'k','LineWidth',2);
    daspect([1 1 1]);
    xlabel('x');
    ylabel('y');
    zlabel('z');
    
    axLength = 5;
    lwidth = 2;
    for idxR=idxsR
        basePointEst = estPos(idxR,:)';
        basePointTrue = truePos(idxR,:)';
        endPointXEst = basePointEst+estRs{idxR}(:,1)*axLength;
        endPointYEst = basePointEst+estRs{idxR}(:,2)*axLength;
        endPointZEst = basePointEst+estRs{idxR}(:,3)*axLength;
        xaxest = [basePointEst'; endPointXEst'];
        yaxest = [basePointEst'; endPointYEst'];
        zaxest = [basePointEst'; endPointZEst'];
        plot3(xaxest(:,1), xaxest(:,2), xaxest(:,3),'Color',estAxisColor,'LineWidth',lwidth);
        plot3(yaxest(:,1), yaxest(:,2), yaxest(:,3),'Color',estAxisColor,'LineWidth',lwidth);
        plot3(zaxest(:,1), zaxest(:,2), zaxest(:,3),'Color',estAxisColor,'LineWidth',lwidth);
        
        endPointXTrue = basePointTrue+trueRs{idxR}(:,1)*axLength;
        endPointYTrue = basePointTrue+trueRs{idxR}(:,2)*axLength;
        endPointZTrue = basePointTrue+trueRs{idxR}(:,3)*axLength;
        xaxtrue = [basePointTrue'; endPointXTrue'];
        yaxtrue = [basePointTrue'; endPointYTrue'];
        zaxtrue = [basePointTrue'; endPointZTrue'];
        plot3(xaxtrue(:,1), xaxtrue(:,2), xaxtrue(:,3),'Color',trueAxisColor,'LineWidth',lwidth);
        plot3(yaxtrue(:,1), yaxtrue(:,2), yaxtrue(:,3),'Color',trueAxisColor,'LineWidth',lwidth);
        plot3(zaxtrue(:,1), zaxtrue(:,2), zaxtrue(:,3),'Color',trueAxisColor,'LineWidth',lwidth);
    end
    
    mins = min(mins, min(min(estPos,[],1)', min(truePos,[],1)'));
    maxs = max(mins, max(max(estPos,[],1)', max(truePos,[],1)'));
    
    if(showGraphs)
        delta = estPos-truePos;
        figure;
        titles = ['x', 'y', 'z'];
        for i=1:3
            subplot(2,2,i);
            plot(idxs, truePos(:,i),'b');
            hold on;
            plot(idxs, estPos(:,i),'r--');
            title(titles(i));
        end
        
        figure;
        titles = ['x', 'y', 'z'];
        deltas = zeros(size(truePos));
        for i=1:length(trueRs)
            deltas(i,:) = (inv(trueRs{i})*(truePos(i,:)-estPos(i,:))')';
        end
        for i=1:3
            subplot(2,2,i);
            plot(idxs, deltas(:,i));
            title(titles(i));
        end
    end
end

mins = mins-1;
maxs = maxs+5;
for i=1:nTypes
    subplot(1,nTypes,i);
    xlim([mins(1) maxs(1)]);
    ylim([mins(2) maxs(2)]);
    zlim([mins(3) maxs(3)]);
    view([-75 10]);
%     cpos = campos;
%     cpos(3) = cpos(3)-300;
%     campos(cpos);
    
    box on;
    grid on;
    
end
set(figHandle, 'Position', [100 100 900 240]);
tightfig;