clear; clc; close all;

addpath('../');

params = initParamsForSimulation;
params.axialSigma = 15;

params.n = 400;
dt = params.dt;

% generate fake us
u = {};
t = 0;
params.dt = 0.005;
for i=1:params.n
    u = vertcat(u, commandFcn(t, params));
    t = t+params.dt;
end

% gridSz = 2 cm;
gridSz = 20;
nParticlesPerDim = 100;


xcurr.q = RotationMatrixToQuat(Ry(pi/2));
xcurr.pos = [0; 0; 0];
xcurr.rho = 40;        % 10 cm roc
xcurr.w = 1;           % not a particle but using the same propagation routine.

xsb = propagateNeedleBack(xcurr, u, params);
xsf = propagateNeedleForward(xcurr, u, params);

params.n = 50;
params.dt = dt;

u = u(1:params.n);


center = xcurr.pos;
%center(1) = center(1)-10;

particles = {};
posx = linspace(center(1)-gridSz/2, center(1)+gridSz/2, nParticlesPerDim);
posy = linspace(center(2)-gridSz/2, center(2)+gridSz/2, nParticlesPerDim);

%look up table for quaternions
lookupTableX = zeros(length(posx),1);

xsl = horzcat(xsb,xsf);

xval = cell2mat(xsl); xval = [xval.pos]; xval = xval(1,:);

for i=1:length(posx)
    [minVal minIdx] = min(abs(xval-posx(i)*ones(size(xval))));
    lookupTableX(i) = minIdx;
end

xs = propagateNeedleBack(xcurr, u, params);



[positionsx positionsy] = meshgrid(posx, posy);
positionsxx = positionsx(:);
positionsyy = positionsy(:);
for i=1:length(positionsxx)
    [yy xx] = ind2sub(size(positionsx), i);
    particles{i}.q = xsl{lookupTableX(xx)}.q;
    particles{i}.pos = [positionsxx(i); positionsyy(i); 0];
    particles{i}.rho = xs{1}.rho;
    particles{i}.w = 1/length(positionsxx);
end



params.p1.uvOffsetSigma = eye(2)*params.mpp*30*1e-3
params.measurementOffsetSigma = zeros(2,2);
% params.onNeedleDopplerMu = 2.3455942812457916254579942197096;
% params.onNeedleDopplerSigma = 0.20738656719215432695570750167604;
% params.offNeedleDopplerMu = 1.7734450777207952976725841602777;
% params.offNeedleDopplerSigma = 1.3317075796588361003232373858813;
%params.usw = params.usw*100;
%params.ush = params.ush*100;

ts = [2 2.75 2.85];
figHandle = figure;
for i=1:3
    measurement = generateRandomMeasurement(xs, u, ts(i), params);
    %particlesBackup = particles;
    particles = measureParticles(particles, u, xcurr, {measurement}, params);
    partsW = zeros(size(positionsx));
    subplot(1,3,i);
    for i=1:length(positionsxx)
        [r c] = ind2sub(size(positionsx), i);
        partsW(r,c) = particles{i}.w;
    end
    h = imagesc(posx, posy, partsW);
    colorbar;
    set(gca, 'FontSize', 12, 'FontName', 'Times New Roman');
    xlim([min(positionsxx) max(positionsxx)]);
    ylim([min(positionsyy) max(positionsyy)]);
    daspect([1 1 1/1000])
    rotate(h, [0 0 1], 270);
    view(2);
    %particles = particlesBackup;
    %t = t+0.5;
end

set(figHandle, 'Position', [100 100 3000 500]);

basePath = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Papers\NeedleEstimationJournal\Figures\';

path = strcat(basePath, 'measurement_prob1.pdf');
export_fig('-transparent', path);

% for x=center(1)-gridSz/2:gridSz/nParticlesPerDim:center(1)+gridSz/2
%     for y=center(2)-gridSz/2:gridSz/nParticlesPerDim:center(2)+gridSz/2
%         particle.pos = [x; y; 0];
%         particle.q = xs{1}.q;
%         particle.rho = 60;
%         particle.w = 1;
%         particles = vertcat(particles, particle);
%     end
% end


