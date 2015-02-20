% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 1;
params.np = 200;
params.n = 40;
params.simulationTime = 8;
params.particleInitTime = 4;
params.drawParticlePos = 1;
params.drawParticleOrientation = 0;
params.axialSigma = 20;
params.axialMu = 0;
params.doRandomTransducerPositioning = 0;

params.initPosSigma = diag([20 20 20]);
params.initPosMu = [0; 0; 20];                                               %pos mu for initial distribution of particles
params.sigmaVelocity = 10;

results = runSimulation(params, @(t,params)(commandFcn(t, params)));

plotParticleFilterResults(results,2);