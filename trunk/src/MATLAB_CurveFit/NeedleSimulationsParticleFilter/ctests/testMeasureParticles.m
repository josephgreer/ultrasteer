clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

params.dt = 1/10;
params.sigmaRho = 0;
params.p1.initRhoSigma = 0;
params.p3.initRhoSigma = 0;
params.doParticleFilter = 1;
params.doMeasurement = 1;
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
    
    close all;
    params.simulationTime = 6;
    [results, xhist, u] = runSimulation(params, @(t,params)commandFcn(t,params));
    
    pathBase = sprintf('./data/testMeasure%d2', method);
    saveCommands(u, pathBase, params);
    saveMeasurements(results.measurements, pathBase, params);
    saveParticles(results.particles{end}, pathBase, params);
end
