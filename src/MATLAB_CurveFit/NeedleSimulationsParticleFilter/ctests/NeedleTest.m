% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;
rng(5);

addpath('../');

basePath = 'C:\Joey\Data\1_7_16\05mm\Trial1\results\';
method = 'Marginalized';

% init params
params = initParamsForSimulation();

params.particleFilterMethod = 3;
% custom parameters
params.doParticleFilter = 1;
params.doMeasurement = 1;
params.np = 500;
params.n = 15;
params.particleInitTime = 2;
params.drawParticlePos = 1;
params.drawParticleOrientation = 0;
params.drawExpectedOrientation = 1;
params.drawParticlesNs = 20;
params.drawUSFrame = 0;
params.drawMeasurement = 1;
params.drawExpectedPos = 1;
params.axialSigma = 10;
params.axialMu = 0;
params.doRandomTransducerPositioning = 0;
params.neff = 0.5;

%params.videoFile = 'C:/needle_particleFilter2.avi';
%params.writeVideo = 0;


%params.sigmaOrientation = diag(0*[pi/1000, pi/1000, pi/1000]);            %orientation process noise std
params.p1.initOrientationSigma = diag(1/10000*[pi, pi, pi]);
params.sigmaRho = 0;
params.p1.initRhoSigma = 0;
params.sigmaOrientation = diag(1/5000*[pi, pi, pi]);
params.measurementOffsetSigma = diag([params.mpp*5*1e-3 params.mpp*5*1e-3])
%params.sigmaPos = diag([0; 0; 0]);
%params.p1.initPosSigma = diag(0*[20 20 20]);
%params.p1.initPosMu = [0; 0; 0];                                    %pos mu for initial distribution of particles
%params.sigmaVelocity = 50;
%params.sigmaRho = 0;
%params.doMeasurement = 0;
nTrials = 1;
params = loadParams(params, basePath, method);


commands = loadCommands(basePath);
measurements = loadMeasurements(basePath);
dts = loadDts(basePath);
trueStates = loadTruthsFromBasePath(strcat(basePath,method,'GroundTruth'));

data.measurements = measurements;
data.dts = dts;
data.trueStates = trueStates;

posErrors = [];
orErrors = [];
for i=1:nTrials
    close all;
    [results, ~, ~] = runSimulation(params, @(t,params)(needleTestCommand(t, params, commands, dts)),data);
    [pE, oE] = plotParticleFilterResults(results,2,params);
    posErrors = vertcat(posErrors, pE);
    orErrors = vertcat(orErrors, oE);
end

