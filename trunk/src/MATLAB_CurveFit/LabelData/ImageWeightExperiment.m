close all;
clearvars -except ips oracleModel ipc;
clc;
fname = 'Y:/NeedleScan/Feb13_LiverScan/Scan 1/scan';

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

range = MapRangeFromDescriptors(oracleModel.descriptor, oracleModel.model, oracleModel.range, descriptor);

ImageWeightExperimentCore;
