needlePtDists = [];
meanErrors = [];
sigmaErrors = [];
transducerAngles = [];
curvatures = [];

figHandles = [];

%% Scan 1
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 1/scan';
load('Feb13/Scan1.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan1_Centroids_Corrected.mat');
load('Feb13/Scan1_Dumped.mat');

%ips = ips([1:size(ips,1)-10],:);

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
clc;

initFrames = min(50, size(ipc,1));
minPoints = 30;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;
minMaxDist = .4;
initMaxDist = 3;

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1;0;0];         %% descriptor variable
descriptor.offset = [0;0;0];
excludeFrames = [];%[73:97];

ImageWeightExperimentCore;
useMin = true;
load('Feb13/Scan1.mat');
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
 transducerAngles = [transducerAngles; max(angles)];

figure(25);
camroll(90);
grid on;

RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];
%% Scan 2
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 2/scan';
load('Feb13/Scan2.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan2_Centroids_Corrected.mat');
load('Feb13/Scan2_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
clc;

initFrames = min(50, size(ipc,1));
minPoints = 30;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;
minMaxDist = .5;
initMaxDist = 10;

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1;0;0];         %% descriptor variable
descriptor.offset = [0;0;0];
excludeFrames = [];%[73:97];

ImageWeightExperimentCore;
useMin = true;
load('Feb13/Scan2.mat');
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
 transducerAngles = [transducerAngles; max(angles)];

RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];


%% Scan 3
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 3/scan';
load('Feb13/Scan3.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan3_Centroids_Corrected.mat');
load('Feb13/Scan3_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
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
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
 transducerAngles = [transducerAngles; max(angles)];

RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];


%% Scan 4
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 4/scan';
load('Feb13/Scan4.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan4_Centroids_Corrected.mat');
load('Feb13/Scan4_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
clc;

initFrames = min(50, size(ipc,1));
minPoints = 30;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;
minMaxDist = .75;
initMaxDist = 5;

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1;0;0];         %% descriptor variable
descriptor.offset = [0;0;0];
excludeFrames = [];%[73:97];

ImageWeightExperimentCore;
load('Feb13/Scan4.mat');
useMin = true;
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
 transducerAngles = [transducerAngles; max(angles)];

RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];

%% Scan 5
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 5/scan';
load('Feb13/Scan5.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan5_Centroids_Corrected.mat');
load('Feb13/Scan5_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
clc;

initFrames = min(50, size(ipc,1));
minPoints = 30;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;
minMaxDist = 1.5;
initMaxDist = 3;

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1;0;0];         %% descriptor variable
descriptor.offset = [0;0;0];
excludeFrames = [];%[73:97];

ImageWeightExperimentCore;
load('Feb13/Scan5.mat');
useMin = true;
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
 transducerAngles = [transducerAngles; max(angles)];


RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];
%% Scan 6
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 6/scan';
load('Feb13/Scan6.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan6_Centroids_Corrected.mat');
load('Feb13/Scan6_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
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
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
transducerAngles = [transducerAngles; max(angles)];
grid on;



RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];

%% Scan 7
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 7/scan';
load('Feb13/Scan7.mat');
FitCurveToLabeledImagePoints;
load('Feb13/Scan7_Centroids_Corrected.mat');
load('Feb13/Scan7_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
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
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
 transducerAngles = [transducerAngles; max(angles)];

figure(25);
camroll(90);
grid on;


RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];
%% Scan 8
% fname = 'Y:/NeedleScan/Jan21_LiverScan/Scan 1/scan';
% load('Jan21/LiverScan/Scan1.mat');
% FitCurveToLabeledImagePoints;
% load('Jan21/LiverScan/Scan1_Centroids_Corrected.mat');
% load('Jan21/LiverScan/Scan1_Dumped.mat');
% 
% close all;
% %clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
% clc;
% 
% initFrames = min(50, size(ipc,1));
% minPoints = 30;         %% minimum frames to start fitting a model to
% maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
% modelGrowthRate = .1;
% modelWeight = 10;
% imageWeight = 1;
% minMaxDist = 1.5;
% initMaxDist = 5;
% 
% degree = 3;             %% polynomial degree to fit to needle
% descriptor.dir = [1;0;0];         %% descriptor variable
% descriptor.offset = [0;0;0];
% excludeFrames = [];%[73:97];
% 
% 
% ImageWeightExperimentCore;
% load('Jan21/LiverScan/Scan1.mat');
% useMin = true;
% ScoreNeedleTipDist;
% needlePtDists = [needlePtDists; needlePointDist];
% meanErrors = [meanErrors; meanErrorsLabeled];
%sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
% AverageTransducerAngle;
%  transducerAngles = [transducerAngles; max(angles)];
% 
% RadiusOfCuravture;
% curvatures = [curvatures; mean(rhos)];

%% Scan 9
fname = 'Y:/NeedleScan/Jan21_LiverScan/Scan 2/scan';
load('Jan21/LiverScan/Scan2.mat');
FitCurveToLabeledImagePoints;
load('Jan21/LiverScan/Scan2_Centroids_Corrected.mat');
load('Jan21/LiverScan/Scan2_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
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
load('Jan21/LiverScan/Scan2.mat');
useMin = true;
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
 transducerAngles = [transducerAngles; max(angles)];

RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];

%% Scan 10
fname = 'Y:/NeedleScan/Jan21_LiverScan/Scan 3/scan';
load('Jan21/LiverScan/Scan3.mat');
FitCurveToLabeledImagePoints;
load('Jan21/LiverScan/Scan3_Centroids_Corrected.mat');
load('Jan21/LiverScan/Scan3_Dumped.mat');

close all;
%clearvars -except ips oracleModel ipc fname needlePtDists meanErrors sigmaErrors transducerAngles;
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

% ips = ipc;
% minMaxDist = 100;
% initMaxDist = 100;
ImageWeightExperimentCore;
load('Jan21/LiverScan/Scan3.mat');
useMin = true;
ScoreNeedleTipDist;
needlePtDists = [needlePtDists; needlePointDist];
meanErrors = [meanErrors; meanErrorsLabeled];
sigmaErrors = [sigmaErrors; sigmaErrorsLabeled];
AverageTransducerAngle;
 transducerAngles = [transducerAngles; max(angles)];

RadiusOfCuravture;
curvatures = [curvatures; mean(rhos)];

%%
clc;
for i=1:size(needlePtDists)
    display(sprintf('%d \t \t & %.2f \t \t & %.3f \t \t & %.2f \\\\', i, needlePtDists(i), meanErrors(i), 1));
    %display(str);
    %display(strcat(num2str(i), '\t \t & ', num2str(needlePtDists(i)), ' \t \t & ', num2str(meanErrors(i)), '\t \t & 1'));
end

str = '';
for i=1:size(needlePtDists)
    str = sprintf('%s \t \t & %d', str, i);
end
display(strcat(str, '\\ \hline'));
str = '';
for i=1:size(needlePtDists)
    str = sprintf('%s \t \t & %.2f', str, needlePtDists(i));
end
display(strcat(str, '\\ \hline'));
str = '';
for i=1:size(needlePtDists)
    str = sprintf('%s \t \t & %.3f', str, meanErrors(i));
end
display(strcat(str, '\\ \hline'));
str = '';
for i=1:size(needlePtDists)
    str = sprintf('%s \t \t & %.3f', str, sigmaErrors(i));
end
display(strcat(str, '\\ \hline'));
str = '';
for i=1:size(needlePtDists)
    str = sprintf('%s \t \t & %.2f', str, 1);
end
display(strcat(str, '\\ \hline'));
str = '';
for i=1:size(needlePtDists)
    str = sprintf('%s \t \t & %.2f', str, curvatures(i)/10);
end
display(strcat(str, '\\ \hline'));


display(strcat('mean needle pt dists:  ', num2str(mean(needlePtDists))));
display(strcat('standard dev needle pt dists:  ', num2str(std(needlePtDists))));
display(strcat('mean pt dists:  ', num2str(mean(meanErrors))));
display(strcat('standard dev pt dists:  ', num2str(mean(sigmaErrors))));

execTimes = [5.16	 	 ; 6.14	 	 ;6.89	 	 ;7.58 	 	 ; 6.64 	 	 ; 5.97 	 	 ; 7.12 	 	 ; 8.53 	 	 ; 7.93 	 	 ; 9.26];
display(strcat('mean execTimes:  ', num2str(mean(execTimes))));
display(strcat('standard dev execTimes:  ', num2str(std(execTimes))));





