% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 0;

global flipped;
flipped = false;

runSimulation(params, @(t,params)(commandFcn(t, params)));