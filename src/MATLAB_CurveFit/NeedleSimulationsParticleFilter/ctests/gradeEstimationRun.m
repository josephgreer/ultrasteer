clearvars -except 'truePosB' 'estPosB'; clc; close all;

method = 'FullState';
%method = 'Marginalized';
estBasePath = strcat('C:\Joey\Data\9_18_15\05mm\Trial4\', method, 'Estimated');
truthBasePath = strcat('C:\Joey\Data\9_18_15\05mm\Trial4\', method, 'GroundTruth');

estPos = load(strcat(estBasePath, 'Pos.dat'));
truePos = load(strcat(truthBasePath, 'Pos.dat'));

estRs = loadOrientations(strcat(estBasePath, 'Rs.dat'));
trueRs = loadOrientations(strcat(truthBasePath, 'Rs.dat'));

estRhos = load(strcat(estBasePath, 'Rho.dat'));
trueRhos = load(strcat(truthBasePath, 'Rho.dat'));

startId = 20;
endId = size(estPos,1)-1;

estPos = estPos([startId:endId], :);
truePos = truePos([startId:endId], :);

estRs = estRs([startId:endId]);
trueRs = trueRs([startId:endId]);

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
    plot(1:size(truePos,1), truePos(:,i),'b');
    hold on;
    plot(1:size(estPos,1), estPos(:,i),'r--');
    title(titles(i));
end

mean(sqrt(sum(delta.^2,2)))