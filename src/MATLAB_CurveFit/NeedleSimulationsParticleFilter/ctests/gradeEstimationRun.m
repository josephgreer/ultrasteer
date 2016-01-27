clearvars -except 'truePosB' 'estPosB'; clc; close all;

method = 'FullState';
%method = 'Marginalized';
trial = 1;
date = '1_7_16'
speed = '05mm';
estBasePath = strcat('C:\Joey\Data\', date, '\', speed, '\Trial', num2str(trial), '\resultsNoDoppler\', method, 'Estimated');
truthBasePath = strcat('C:\Joey\Data\', date, '\', speed, '\Trial', num2str(trial), '\resultsNoDoppler\', method, 'GroundTruth');
% estBasePath = strcat('C:\Joey\Data\', date, '\', speed, '\Trial', num2str(trial), '\', method, 'Estimated');
% truthBasePath = strcat('C:\Joey\Data\', date, '\', speed, '\Trial', num2str(trial), '\', method, 'GroundTruth');
estPos = load(strcat(estBasePath, 'Pos.dat'));
truePos = load(strcat(truthBasePath, 'Pos.dat'));

estRs = loadOrientations(strcat(estBasePath, 'Rs.dat'));
trueRs = loadOrientations(strcat(truthBasePath, 'Rs.dat'));
estRhos = load(strcat(estBasePath, 'Rho.dat'));
trueRhos = load(strcat(truthBasePath, 'Rho.dat'));

[indicesR, indicesC] = ind2sub(size(estPos), find(estPos ~= 0));
startId = min(indicesR);
endId = size(estPos,1)-20;
idxs = [startId:endId];

estPos = estPos(idxs, :);
truePos = truePos(idxs, :);

estRs = estRs(idxs);
trueRs = trueRs(idxs);

scatter3(estPos(:,1), estPos(:,2), estPos(:,3), 'b');
hold on;
scatter3(truePos(:,1), truePos(:,2), truePos(:,3), 'r');
daspect([1 1 1]);
xlabel('x');
ylabel('y');
zlabel('z');

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

% figure;
% titles = ['x', 'y', 'z'];
% estDeltas = zeros(size(estPos,1)-1, 3);
% for i=2:length(trueRs)
%     estDeltas(i,:) = (inv(trueRs{i})*(estPos(i,:)-estPos(i-1,:))')';
% end
% for i=1:3
%     subplot(2,2,i);
%     plot(idxs, estDeltas(:,i));
%     title(titles(i));
% end

mean(sqrt(sum(delta.^2,2)))