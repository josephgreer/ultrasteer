clear; clc; close all;
addpath('../');

useMarg = 1;
if(useMarg)
    method = 'Marginalized';
else
    method = 'FullState';
end
baseDir = './data/';
basePath = strcat(baseDir, 'testCMeasure');
params = initParamsForSimulation();
if(useMarg)
    params.particleFilterMethod = 3;
end
params = loadParams(params, baseDir, method);
params.drawTruePos = 0;
params.np = 500;
params.drawParticlePos = 1;
params.LUTDistributionBasePath = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/NeedleScan/1_7_16/05mm/Trial1/';

rng('default');
rng(1);

figure(1);
hold on;
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