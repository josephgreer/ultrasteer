% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 1;
params.doMeasurement = 1;
params.np = 1000;
params.n = 20;
params.simulationTime = 12;
params.particleInitTime = 2;
params.drawParticlePos = 1;
params.drawParticleOrientation = 0;
params.drawExpectedOrientation = 1;
params.drawParticlesNs = 20;
params.drawUSFrame = 1;
params.drawMeasurement = 1;
params.drawExpectedPos = 1;
params.axialSigma = 10;
params.axialMu = 0;
params.doRandomTransducerPositioning = 0;
params.neff = 0.5;

%params.videoFile = 'C:/needle_particleFilter2.avi';
%params.writeVideo = 0;



%params.sigmaOrientation = diag(0*[pi/1000, pi/1000, pi/1000]);            %orientation process noise std
params.p1.initOrientationSigma = diag(1/2000*[pi, pi, pi]);
params.sigmaRho = 0;
params.p1.initRhoSigma = 0;
params.sigmaOrientation = diag(1/5000*[pi, pi, pi]);
%params.sigmaPos = diag([0; 0; 0]);
%params.p1.initPosSigma = diag(0*[20 20 20]);
%params.p1.initPosMu = [0; 0; 0];                                    %pos mu for initial distribution of particles
%params.sigmaVelocity = 50;
%params.sigmaRho = 0;
%params.doMeasurement = 0;
[results, ~, ~] = runSimulation(params, @(t,params)(commandFcn(t, params)));

plotParticleFilterResults(results,2);