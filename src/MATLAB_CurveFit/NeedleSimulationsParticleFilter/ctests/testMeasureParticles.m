clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

params.dt = 1/10;
params.p1.initRhoSigma = 0;
params.p3.initRhoSigma = 0;
params.p3.distanceThresh = 50^2;
params.doParticleFilter = 1;
params.doMeasurement = 0;
params.n = 50;

for method=[1,3]
    close all;
    params.simulationTime = 5;
    params.particleFilterMethod = method;
    [results, xhist, u] = runSimulation(params, @(t,params)commandFcn(t,params));
    
    pathBase = sprintf('./data/testMeasure%d1', method);
    saveCommands(u, pathBase, params);
    saveMeasurements(results.measurements, pathBase, params);
    saveParticles(results.particles{end}, pathBase, params);

    dts = repmat(params.dt, length(u), 1);
    
    xp = measureParticles(results.particles{end}, u, xhist, dts, flip(results.measurements), params);
    saveParticles(xp, sprintf('./data/testMeasureAfter%d1', method), params);
    
    close all;
    params.simulationTime = 6;
    params.p3.subsetSize = 2000;
    [results, xhist, u] = runSimulation(params, @(t,params)commandFcn(t,params));
    
    pathBase = sprintf('./data/testMeasure%d2', method);
    saveCommands(u, pathBase, params);
    saveMeasurements(results.measurements, pathBase, params);
    saveParticles(results.particles{end}, pathBase, params);
    
    dts = repmat(params.dt, length(u), 1);
    xp = measureParticles(results.particles{end}, u, xhist, dts, flip(results.measurements), params);
    saveParticles(xp, sprintf('./data/testMeasureAfter%d2', method), params);
end
