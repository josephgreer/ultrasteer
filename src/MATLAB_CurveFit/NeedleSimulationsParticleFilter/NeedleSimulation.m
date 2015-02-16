% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 1;
params.np = 50;
params.simulationTime = 8;

%params.initPosMu = [10; 10; 10];                                               %pos mu for initial distribution of particles

results = runSimulation(params, @(t,params)(commandFcn(t, params)));

plotParticleFilterResults(results,2);