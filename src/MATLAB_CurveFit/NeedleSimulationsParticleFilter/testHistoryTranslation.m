% generate needle trajectory
clear; clc; close all;

addpath('icp');

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 0;
params.doMeasurement = 0;
params.simulationTime = 12;
params.particleInitTime = 100;

params.sigmaOrientation = diag(1/5*[1*pi/1000, 1*pi/1000, pi/1000]);            
%params.sigmaPos = zeros(3,3);
%params.sigmaRho = 0;
%params.sigmaRho = 1;
%params.sigmaVelocity = 0;

params.n = 20;
nskip = 1;

[~, xhist, u] = runSimulation(params, @(t,params)(twistyCommandFcn(t, params)));


xref = propagateNeedleBack(xhist{1}, u, params);
xrefPos = cell2mat(xref);
xrefPos = [xrefPos.pos];
for i=1:20
    xcurr = xhist{1};
    dp = rand(3,1)*5;
    xcurr.pos = xcurr.pos+dp;
    xp = propagateNeedleBack(xcurr, u, params);
    xpPos = cell2mat(xp);
    xpPos = [xpPos.pos];
    xpPosShifted = xrefPos+repmat(dp,1,size(xpPos,2));
    max(max(abs(xpPosShifted-xpPos)))
    %drawPointHistory(xp(1:nskip:end), 1, [0 0 1]);
end



%drawFrames(1, {xp{1}}, 5, []); 
