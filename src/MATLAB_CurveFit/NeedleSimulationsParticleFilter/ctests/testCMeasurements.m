clear; clc; close all;
addpath('../');

yep =   [-615.74299609414          222.274648570614        522.582079556706];
params = initParamsForSimulation();
params.useLUTDistribution = 1;
params.drawTipFrame = 1;
params.drawUSFrame = 1;
params.drawTruePos = 0;
%params.drawParticlePos = 1;
params.drawExpectedPos = 1;
params.drawExpectedOrientation = 1;
params.mpp = 163;
params.p1.uvOffsetSigma = diag([params.mpp*5*1e-3 params.mpp*5*1e-3]);  %5 pixels of measurement noise

params.sigB0 = -2.3790178529765900;
params.sigB1 = 0.053473668798548403;

rng('default');
rng(1);

figure(1);
hold on;
basePath = './data/testCMeasure';
measurements = loadMeasurements(basePath);
xp = loadParticlesFromBasePath(basePath, params);
dts = loadDts(basePath);
us = loadCommands(basePath);

params.usw = norm(measurements{1}.fur-measurements{1}.ful);
params.ush = norm(measurements{1}.fur-measurements{1}.fbr);

xpe = expectedValueOfParticles(xp, params);
usFrameHandles = drawUSFrame(measurements{1}, params, []);
particleHandles = drawParticles(1, xp, xpe, [], params, []);
%tipFrameHandles = drawFrame(xpe, 1, []);
daspect([1 1 1]);
view(-109, 32);
campos(yep);

xpa = measureParticles(xp, us, [], dts, measurements, params);

xpea = expectedValueOfParticles(xpa, params);
particleHandles = drawParticles(1, xpa, xpea, [], params, particleHandles);
campos(yep);
daspect([1 1 1]);
%tipFrameHandles = drawFrame(1, xpea, 5, params, tipFrameHandles);