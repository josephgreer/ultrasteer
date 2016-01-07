% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% init params
params = initParamsForSimulation();


nTrials = 10;

totalErrors = [];
for trial = 1:nTrials
    close all;

    % custom parameters
    params.doParticleFilter = 1;
    params.doMeasurement = 1;
    params.np = 1;
    params.n = 50;
    params.particleInitTime = 2;
    params.drawParticlesNs = 20;
    params.drawUSFrame = 1;
    params.drawMeasurement = 1;
    params.axialSigma = 10;
    params.axialMu = -5;
    params.sigmaRho = 0;
    params.sigmaOrientation = diag(1/5000*[pi,pi,pi]);
    params.doRandomTransducerPositioning = 0;
    params.neff = 0;
    params.particleFilterMethod = 100;
    %params.p100.measurementSigma = zeros(3,3);
    params.p100.measurementSigma = diag([5e-3 5e-3 0.5]);
    %params.measurementOffsetSigma = zeros(2,2);
    params.measurementOffsetSigma = diag([params.mpp*5*1e-3 params.mpp*5*1e-3]);
    params.simulationTime = 15;
    params.p100.subsetSize = 15;
    params.p100.procrustesIt = 3;
    params.drawExpectedOrientation = 1;

    [results, ~, ~] = runSimulation(params, @(t,params)(commandFcn(t, params)),[]);

    Rerrors = zeros(length(results.states), 3);
    for i=1:length(results.states)
        Rp = results.particles{i}{1}.dist.mu;
        R = QuatToRotationMatrix(results.states{i}.q);
        Rerrors(i,:) = SO3HatInverse(SO3Log(R'*Rp))';
    end

    figure(4);
    subplot(2,2,1);
    plot([1:size(Rerrors,1)], Rerrors(:,1));
    xlim([0 100]);
    subplot(2,2,2);
    plot([1:size(Rerrors,1)], Rerrors(:,2));
    xlim([0 100]);
    subplot(2,2,3);
    plot([1:size(Rerrors,1)], Rerrors(:,3));
    xlim([0 100]);
    subplot(2,2,4);
    plot([1:size(Rerrors,1)], sqrt(sum(Rerrors.^2,2)));
    xlim([0 100]);

    Rerrors2 = Rerrors.^2;
    Rerrors2 = Rerrors2(params.p100.minimumMeasurements:end, :);
    totalErrors = vertcat(totalErrors, mean(Rerrors2,1));
end
%plotParticleFilterResults(results,2);