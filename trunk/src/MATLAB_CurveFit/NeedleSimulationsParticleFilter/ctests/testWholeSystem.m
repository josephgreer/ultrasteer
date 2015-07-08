clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

params.dt = 1/10;
params.p1.initRhoSigma = 0;
params.p3.initRhoSigma = 0;
params.doParticleFilter = 1;
params.sigmaRho = 0;
params.n = 50;

nTests = 1;

for method=[1,3]
    for test=1:nTests
        close all;
        params.simulationTime = 15;
        params.particleFilterMethod = method;
        [results, xhist, u] = runSimulation(params, @(t,params)commandFcn(t,params));
        [pE, oE] = plotParticleFilterResults(results,2,params);
        
        pathBase = sprintf('./data/testWholeSystem%d%d', method,test);
        saveCommands(results.us, pathBase, params);
        saveMeasurements(results.measurements, pathBase, params);
        particleFilterMethod = params.particleFilterMethod;
        params.particleFilterMethod = 1;
        saveParticles(results.states, pathBase, params);
        params.particleFilterMethod = particleFilterMethod;
        
        time = results.time;
        save(strcat(pathBase, 'dts.dat'), 'time', '-ascii');
    end
    
end
