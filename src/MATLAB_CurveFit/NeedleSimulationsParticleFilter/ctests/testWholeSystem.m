clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

params.dt = 1/10;
params.p1.initRhoSigma = 0;
params.p3.initRhoSigma = 0;
params.doParticleFilter = 1;
params.sigmaRho = 3;
params.n = 50;
params.np = 200;
params.doParticleFilter = 1;

nTests = 1;

for method=[3]
    for test=1:nTests
        close all;
        params.simulationTime = 50;
        params.particleFilterMethod = method;
        [results, xhist, u] = runSimulation(params, @(t,params)commandFcn(t,params),[]);
        [pE, oE] = plotParticleFilterResults(results,2,params);
        
        pathBase = sprintf('./data/testWholeSystem1%d',test);
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
