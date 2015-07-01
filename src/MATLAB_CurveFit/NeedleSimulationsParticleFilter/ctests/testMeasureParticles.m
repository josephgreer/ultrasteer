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
    %measureParticles(results.particles{end}, u, results.stats{end}, results.measurements, params)
    
    close all;
    params.simulationTime = 6.5;
    [results, xhist, u] = runSimulation(params, @(t,params)commandFcn(t,params));
end