clear; clc; close all;
addpath('../');

params = initParamsForSimulation();
rng('default');
rng(1);

x.q = AxisAngleToQuat(rand(3,1)*pi);
x.pos = rand(3,1)*500 - 250
x.rho = 60+20*rand(1)-10
x.w = 1;

x.pos
x.rho
QuatToRotationMatrix(x.q)

uc.dtheta = pi/3;
uc.v = 500;

params.dt = 1/10;
params.sigmaRho = 0;
params.p1.initRhoSigma = 0;
params.p3.initRhoSigma = 0;

for method=[1,3]
    close all;
    params.particleFilterMethod = method;
    xp = initializeParticles({x}, uc, params);
    
    pos = particlePositions(xp, params);
    rho = particleRhos(xp, params);
    
    basePath = sprintf('./data/propagateParticlesInit%d',method);
    paths = {strcat(basePath, 'Pos.dat')};
    paths = vertcat(paths, {strcat(basePath, 'Rhos.dat')});
    paths = vertcat(paths, {strcat(basePath, 'Rs.dat')});
    if(method == 3)
        paths = vertcat(paths, {strcat(basePath, 'sigmas.dat')});
    end
    
    xpp = loadParticles(params, paths);
    
    ppos = particlePositions(xpp, params);
    prho = particleRhos(xpp, params);
    
    % check that initialized positions look right
    %matlab
    scatter3(pos(:,1), pos(:,2), pos(:,3),'b');
    hold on;
    axis equal;
    pause;
    %c++
    scatter3(ppos(:,1), ppos(:,2), ppos(:,3),'r');
    axis equal;
    
    figure;
    % check that initialized orientations look right
    %matlab
    drawFramesZeroPosition(2, xpp, 15, [], params);
    axis equal;
    view(3);
    grid on;
    hold on;
    pause;
    %c++
    drawFramesZeroPosition(2, xp, 5, [],params);
    pause;
    
    close all;
    
    xpB = propagateParticles(xp, uc, params);
    
    pos = particlePositions(xpB, params);
    rho = particleRhos(xpB, params);
    
    basePath = sprintf('./data/propagateParticlesProp%d',method);
    paths = {strcat(basePath, 'Pos.dat')};
    paths = vertcat(paths, {strcat(basePath, 'Rhos.dat')});
    paths = vertcat(paths, {strcat(basePath, 'Rs.dat')});
    if(method == 3)
        paths = vertcat(paths, {strcat(basePath, 'sigmas.dat')});
    end
    xpp = loadParticles(params, paths);
    
    ppos = particlePositions(xpp, params);
    prho = particleRhos(xpp, params);
    
    close all;
    
    % check that propagated positions look right
    %matlab
    scatter3(pos(:,1), pos(:,2), pos(:,3),'b');
    hold on;
    axis equal;
    pause;
    %c++
    scatter3(ppos(:,1), ppos(:,2), ppos(:,3),'r');
    axis equal;
    
    figure(2);
    % check that propagated orientations look right
    %matlab
    drawFramesZeroPosition(2, xpp, 15, [],params);
    axis equal;
    view(3);
    grid on;
    hold on;
    pause;
    %c++
    drawFramesZeroPosition(2, xpB, 5, [], params);
    pause;
end


