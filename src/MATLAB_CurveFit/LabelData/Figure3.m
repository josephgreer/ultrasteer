%% Scan 6
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 6/scan';
load('Feb13/Scan6.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan6_Centroids_Corrected.mat');
load('Feb13/Scan6_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists rmsErrors transducerAngles;
clc;

ips = ips([1:size(ips,1)-20],:);

initFrames = min(50, size(ipc,1));
minPoints = 30;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;
minMaxDist = 1.5;
initMaxDist = 10;

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1;0;0];         %% descriptor variable
descriptor.offset = [0;0;0];
excludeFrames = [];%[73:97];

ImageWeightExperimentCore;
load('Feb13/Scan6.mat');
figure(25);
useMin = true;
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
rmsErrors = [rmsErrors; rmsErrorsLabeled];
AverageTransducerAngle;
transducerAngles = [transducerAngles; aveAngle];

dists = EvaluateModelOnPoints(model, descriptor, degree, mps(:,[1:3]));

oliers = mps;
oliers(usedIndices,:) = [];
idxs = [1:size(oliers,1)]';
if(size(oliers,1) > 75)
    idxs = round(unifrnd(1, size(oliers,1),[75 1]));
end

view(2);
camroll(90);
box on;
grid on;
axis equal;
%ylim([90 105]);
%xlim([97 117]);

xlabel('x');
ylabel('y');
zlabel('z');

set(gcf, 'color','w');
set(findall(gcf,'-property','FontSize'),'FontSize',12,...
    'fontName','Times New Roman');

delete(hiscatter);

set(gca,'Units','pixels');
set(figure(25), 'Units', 'pixels');
daspect([1 1 1]);
daspect
pbaspect([3 1 1])
export_fig -transparent 'needle1.pdf'

set(gca, 'Position', [10 10 150 450]);
set(figure(25), 'Position', [10 10 190 480]);
set(gca, 'XTickLabel', []);
set(gca, 'YTickLabel', []);
set(gca, 'ZTickLabel', []);
xlim
camorbit(30, 10, 'coordsys', [0 1 0]);


scatter3(oliers(idxs,1),oliers(idxs,2),oliers(idxs,3),60, 'k','d','LineWidth',2);
scatter3(ptsForModel(:,1), ptsForModel(:,2), ptsForModel(:,3), 60, 'b', 'LineWidth', 2);
set(gca, 'Position', [10 10 200 600]);
set(figure(25), 'Position', [10 10 240 630]);
set(gca, 'XTickLabel', []);
set(gca, 'YTickLabel', []);
set(gca, 'ZTickLabel', []);
daspect([1 1 1]);
daspect
pbaspect([3 1 1])
export_fig -transparent 'needle2.pdf'





%% Scan 7
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 7/scan';
load('Feb13/Scan7.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan7_Centroids_Corrected.mat');
load('Feb13/Scan7_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists rmsErrors transducerAngles;
clc;

initFrames = min(50, size(ipc,1));
minPoints = 30;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;
minMaxDist = 1.5;
initMaxDist = 10;

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1;0;0];         %% descriptor variable
descriptor.offset = [0;0;0];
excludeFrames = [];%[73:97];

ImageWeightExperimentCore;
load('Feb13/Scan7.mat');
useMin = true;
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
rmsErrors = [rmsErrors; rmsErrorsLabeled];
AverageTransducerAngle;
transducerAngles = [transducerAngles; aveAngle];

dists = EvaluateModelOnPoints(model, descriptor, degree, mps(:,[1:3]));

oliers = mps;
oliers(usedIndices,:) = [];
idxs = [1:size(oliers,1)]';
if(size(oliers,1) > 75)
    idxs = round(unifrnd(1, size(oliers,1),[75 1]));
end

figure(25);
view(2);
camroll(270);
box on;
grid on;
axis equal;
%ylim([90 105]);
%xlim([97 117]);

xlabel('x');
ylabel('y');
zlabel('z');

set(gcf, 'color','w');
set(findall(gcf,'-property','FontSize'),'FontSize',12,...
    'fontName','Times New Roman');

delete(hiscatter);

set(gca,'Units','pixels');
set(figure(25), 'Units', 'pixels');
daspect([1 1 1]);
daspect
pbaspect([3 1 1])
xlim
export_fig -transparent 'needle3.pdf'

set(gca, 'Position', [10 10 150 450]);
set(figure(25), 'Position', [10 10 190 480]);
set(gca, 'XTickLabel', []);
set(gca, 'YTickLabel', []);
set(gca, 'ZTickLabel', []);
camorbit(30, 10, 'coordsys', [0 1 0]);


scatter3(oliers(idxs,1),oliers(idxs,2),oliers(idxs,3),60, 'k','d','LineWidth',2);
scatter3(ptsForModel(:,1), ptsForModel(:,2), ptsForModel(:,3), 60, 'b', 'LineWidth', 2);
set(gca, 'Position', [10 10 200 600]);
set(figure(25), 'Position', [10 10 240 630]);
set(gca, 'XTickLabel', []);
set(gca, 'YTickLabel', []);
set(gca, 'ZTickLabel', []);
daspect([1 1 1]);
daspect
pbaspect([3 1 1])
export_fig -transparent 'needle4.pdf'
%%
%% Scan 7
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 3/scan';
load('Feb13/Scan3.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan3_Centroids_Corrected.mat');
load('Feb13/Scan3_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists rmsErrors transducerAngles;
clc;

initFrames = min(50, size(ipc,1));
minPoints = 30;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;
minMaxDist = .55;
initMaxDist = 5;

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1;0;0];         %% descriptor variable
descriptor.offset = [0;0;0];
excludeFrames = [];%[73:97];

ImageWeightExperimentCore;
load('Feb13/Scan3.mat');
useMin = true;
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
rmsErrors = [rmsErrors; rmsErrorsLabeled];
AverageTransducerAngle;
transducerAngles = [transducerAngles; aveAngle];


dists = EvaluateModelOnPoints(model, descriptor, degree, mps(:,[1:3]));

oliers = mps;
oliers(usedIndices,:) = [];
idxs = [1:size(oliers,1)]';
if(size(oliers,1) > 100)
    idxs = round(unifrnd(1, size(oliers,1),[100 1]));
end

figure(25);
view(2);
camroll(90);
box on;
grid on;
axis equal;
%ylim([90 105]);
%xlim([97 117]);

xlabel('x');
ylabel('y');
zlabel('z');

set(gcf, 'color','w');
set(findall(gcf,'-property','FontSize'),'FontSize',12,...
    'fontName','Times New Roman');

delete(hiscatter);

set(gca,'Units','pixels');
set(figure(25), 'Units', 'pixels');
daspect([1 1 1]);
daspect
pbaspect([3 1 1])
xlim
export_fig -transparent 'needle3.pdf'

set(gca, 'Position', [10 10 150 450]);
set(figure(25), 'Position', [10 10 190 480]);
set(gca, 'XTickLabel', []);
set(gca, 'YTickLabel', []);
set(gca, 'ZTickLabel', []);
camorbit(30, 10, 'coordsys', [0 1 0]);


scatter3(oliers(idxs,1),oliers(idxs,2),oliers(idxs,3),60, 'k','d','LineWidth',2);
scatter3(ptsForModel(:,1), ptsForModel(:,2), ptsForModel(:,3), 60, 'b', 'LineWidth', 2);
set(gca, 'Position', [10 10 200 600]);
set(figure(25), 'Position', [10 10 240 630]);
set(gca, 'XTickLabel', []);
set(gca, 'YTickLabel', []);
set(gca, 'ZTickLabel', []);
daspect([1 1 1]);
daspect
pbaspect([3 1 1])
export_fig -transparent 'needle4.pdf'

%% Scan 6
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 6/scan';
load('Feb13/Scan6.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan6_Centroids_Corrected.mat');
load('Feb13/Scan6_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists rmsErrors transducerAngles;
clc;

ips = ips([1:size(ips,1)-20],:);

initFrames = min(50, size(ipc,1));
minPoints = 30;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;
minMaxDist = 1.5;
initMaxDist = 10;

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1;0;0];         %% descriptor variable
descriptor.offset = [0;0;0];
excludeFrames = [];%[73:97];

ImageWeightExperimentCore;
load('Feb13/Scan6.mat');
figure(25);
camroll(90);
useMin = true;
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
rmsErrors = [rmsErrors; rmsErrorsLabeled];
AverageTransducerAngle;
transducerAngles = [transducerAngles; aveAngle];
grid on;


dists = EvaluateModelOnPoints(model, descriptor, degree, mps(:,[1:3]));

oliers = mps;
oliers(usedIndices,:) = [];
idxs = [1:size(oliers,1)]';
if(size(oliers,1) > 75)
    idxs = round(unifrnd(1, size(oliers,1),[75 1]));
end

figure(25);
view(2);
camroll(270);
box on;
grid on;
axis equal;
%ylim([90 105]);
%xlim([97 117]);

xlabel('x');
ylabel('y');
zlabel('z');

set(gcf, 'color','w');
set(findall(gcf,'-property','FontSize'),'FontSize',12,...
    'fontName','Times New Roman');

delete(hiscatter);

set(gca,'Units','pixels');
set(figure(25), 'Units', 'pixels');

pbaspect([3 1 1])
xlim([90.5 92]);
set(gca, 'DataAspectRatio', [1 1 1]);

% ylim([99.75 100.25]);
%xlim([145.5 146.5]);
% set(gca, 'Position', [10 10 150 450]);
% set(figure(25), 'Position', [10 10 190 480]);
set(gca, 'XTickLabel', []);
set(gca, 'YTickLabel', []);
set(gca, 'ZTickLabel', []);

export_fig -transparent 'needle5.pdf'

