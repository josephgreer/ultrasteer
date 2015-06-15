% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 1;
params.doMeasurement = 1;
params.np = 1;
params.n = 20;
params.simulationTime = 12;
params.particleInitTime = 2;
params.drawParticlesNs = 20;
params.drawUSFrame = 1;
params.drawMeasurement = 1;
params.axialSigma = 10;
params.axialMu = -10;
params.doRandomTransducerPositioning = 0;
params.neff = 0;
params.particleFilterMethod = 100;
params.p100.measurementSigma = zeros(3,3);
params.measurementOffsetSigma = zeros(2,2);


[results, ~, ~] = runSimulation(params, @(t,params)(commandFcn(t, params)));

plotParticleFilterResults(results,2);