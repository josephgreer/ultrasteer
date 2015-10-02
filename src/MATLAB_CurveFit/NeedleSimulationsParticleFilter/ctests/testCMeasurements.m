clear; clc; close all;
addpath('../');

yep =   [-615.74299609414          222.274648570614        522.582079556706];

params = initParamsForSimulation();
params.useLUTDistribution = 0;
params.drawTipFrame = 1;
params.drawUSFrame = 1;
params.drawTruePos = 0;
params.drawParticlesNs = 1;
params.drawParticlePos = 1;
params.drawParticleOrientation = 0;
params.drawExpectedPos = 1;
params.drawExpectedOrientation = 1;
params.mpp = 119;
params.p1.uvOffsetSigma = diag([params.mpp*15*1e-3 params.mpp*15*1e-3]);  %5 pixels of measurement noise
params.LUTDistributionBasePath = 'C:/Joey/Data/8_24_15/Trial3/Insertion/'

params.sigB0 = -1.6679;
params.sigB1 = 0.0594;

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

% Re = QuatToRotationMatrix(xpe.q);
% projScatter = zeros(length(xp),3);
% for i=1:length(xp)
%     projScatter(i,1) = (xp{i}.pos-xpe.pos)'*Re(:,3);
%     projScatter(i,2) = xp{i}.w;
%     projScatter(i,3) = i;
% end
% scatter(projScatter(:,1), projScatter(:,2));

usFrameHandles = drawUSFrame(measurements{1}, params, []);
particleHandles = drawParticles(1, xp, xpe, [], params, []);
%tipFrameHandles = drawFrame(xpe, 1, []);
daspect([1 1 1]);
view(-109, 32);
campos(yep);

params.n = 500;

xpa = measureParticles(xp, us, [], dts, measurements, params);

xpea = expectedValueOfParticles(xpa, params);
particleHandles = drawParticles(1, xpa, xpea, [], params, particleHandles);

figure;

Re = QuatToRotationMatrix(xpea.q);
projScatter = zeros(length(xpa),3);
for i=1:length(xpa)
    projScatter(i,1) = (xpa{i}.pos-xpea.pos)'*Re(:,3);
    projScatter(i,2) = xpa{i}.w;
    projScatter(i,3) = i;
end
scatter(projScatter(:,1), projScatter(:,2));
indices = sortrows(projScatter);
indices = indices(find(indices(:,1) > 2.5),:);

dot((xpea.pos-xpe.pos),Re(:,3))
% campos(yep);
% daspect([1 1 1]);
%tipFrameHandles = drawFrame(1, xpea, 5, params, tipFrameHandles);