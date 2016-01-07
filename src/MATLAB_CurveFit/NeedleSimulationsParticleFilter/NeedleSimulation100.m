% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();
params.particleFilterMethod = 100;
params.np = 1;
params.drawExpectedOrientation = 1;
params.drawUSFrame = 0 ;
params.drawMeasurement = 0;
params.simulationTime = 30;
params.drawFloatingFrame = 1;

params.videoFile = 'C:/Joey/Data/orientationEstimationWithKalman2.mp4';
params.writeVideo = 1;

%params.p100.measurementSigma = zeros(3,3);
params.sigmaOrientation = diag(1/5000*[pi, pi, pi]);


[results, ~, ~] = runSimulation(params, @(t,params)(twistyCommandFcn(t, params)),[]);