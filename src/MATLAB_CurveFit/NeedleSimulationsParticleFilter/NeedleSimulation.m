% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 1;
params.doMeasurement = 1;
params.np = 200;
params.n = 20;
params.simulationTime = 16;
params.particleInitTime = 2;
params.drawParticlePos = 1;
params.drawParticleOrientation = 0;
params.drawUSFrame = 1;
params.drawMeasurement = 1;
params.drawExpectedPos = 1;
params.axialSigma = 10;
params.axialMu = 0;
params.doRandomTransducerPositioning = 0;
%params.writeVideo = 0;
%params.videoFile = 'C:/needle_particleFilter2.avi';

params.initPosSigma = diag([20 20 20]);
params.initPosMu = [0; 0; 0];                                               %pos mu for initial distribution of particles
results = runSimulation(params, @(t,params)(commandFcn(t, params)));

plotParticleFilterResults(results,2);