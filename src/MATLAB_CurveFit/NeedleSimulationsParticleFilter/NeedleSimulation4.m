% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 1;
params.doMeasurement = 1;
params.particleFilterMethod = 4;
params.np = 200;
params.n = 50;
params.simulationTime = 20;
params.particleInitTime = 2;
params.drawParticlePos = 1;
params.drawParticleOrientation = 0;
params.drawExpectedOrientation = 1;
params.drawParticlesNs = 5;
params.drawUSFrame = 1;
params.drawMeasurement = 1;
params.drawExpectedPos = 1;
params.axialSigma = 10;
params.axialMu = 0;
params.doRandomTransducerPositioning = 0;
params.neff = 0.5;
params.drawParticlesNs = 1;

params.sigmaRho  = 0;
params.p4.initRhoSigma = 0;
params.measurementOffsetSigma = diag([params.mpp*5*1e-3 params.mpp*5*1e-3]);

%params.videoFile = 'C:/needle_particleFilter2.avi';
%params.writeVideo = 0;

params.sigmaOrientation = diag(1/5000*[pi, pi, pi]);


[results, ~, ~] = runSimulation(params, @(t,params)(commandFcn(t, params)),[]);

plotParticleFilterResults(results,2,params);