clear; clc; close all;

rng(1);

addpath('../');

params = initParamsForSimulation;
params.axialSigma = 15;
params.offFrameB0 = -5;
params.offFrameB1 = 5;

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
nParticlesPerDim = 50;


xcurr.q = RotationMatrixToQuat(Ry(pi/2));
xcurr.pos = [0; 0; 0];
xcurr.rho = 20;        % 10 cm roc
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
distances = zeros(length(posx),1);

xsl = horzcat(xsb,xsf);

xval = cell2mat(xsl); xval = [xval.pos]; xval = xval(1,:);
for i=1:length(posx)
    [minVal minIdx] = min(abs(xval-posx(i)*ones(size(xval))));
    lookupTableX(i) = minIdx;
    distances(i) = minVal;
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



params.p1.uvOffsetSigma = eye(2)*params.mpp*15*1e-3
params.measurementOffsetSigma = zeros(2,2);
% params.onNeedleDopplerMu = 2.3455942812457916254579942197096;
% params.onNeedleDopplerSigma = 0.4;%0.20738656719215432695570750167604;
% params.offNeedleDopplerMu = 1.7734450777207952976725841602777;
% params.offNeedleDopplerSigma = 1.3317075796588361003232373858813;
%params.usw = params.usw*100;
%params.ush = params.ush*100;

params.drawUSFrame = 1;
params.drawMeasurement = 0;

ts = [2 2.75 2.9];
figHandle = figure;

zaspects = [1/2000; 1/1000; 1/350];
xhist = cell2mat(xs); xhist = [xhist.pos]';
xhist(find(xhist(:,1) > max(posx)+1),:) = [];
ztop = zeros(3,1);
for i=1:3
    measurement = generateRandomMeasurement(xs, u, ts(i), params);
    %particlesBackup = particles;
    fakeMeas = measurement;
%     fakeMeas.ful = [measurement.pos(1); -params.usw/2; -params.ush/2];
%     fakeMeas.fbl = [measurement.pos(1); -params.usw/2; params.ush/2];
%     fakeMeas.fbr = [measurement.pos(1); params.usw/2; params.ush/2];
%     fakeMeas.fur = [measurement.pos(1); params.usw/2; -params.ush/2];
%     fakeMeas.bx = fakeMeas.fur-fakeMeas.ful; fakeMeas.bx = fakeMeas.bx/norm(fakeMeas.bx);
%     fakeMeas.by = fakeMeas.fbl-fakeMeas.ful; fakeMeas.by = fakeMeas.by/norm(fakeMeas.by);
%     fakeMeas.uv = [fakeMeas.bx fakeMeas.by]\(fakeMeas.pos-fakeMeas.ful);
    
    
    particles = measureParticles(particles, u, xcurr, {fakeMeas}, params);
    partsW = zeros(size(positionsx));
    
    subplot(1,3,i);
    for j=1:length(positionsxx)
        [r c] = ind2sub(size(positionsx), j);
        partsW(r,c) = particles{j}.w;
    end
%     gauss = fspecial('gaussian', [10 10], 2);
%     partsW = imfilter(partsW, gauss);
    h = surf(posx, posy, partsW, 'EdgeColor', 'none', 'LineStyle', 'none');
    cmap = colormap;
    
    alpha(h, 0.4);
    hold on;
    ll = plot3(xhist(:,1), xhist(:,2), xhist(:,3), 'k', 'LineWidth', 2, 'Color', [0.3, 0.3, 0.3]);
    scatter3(xhist(1,1), xhist(1,2), xhist(1,3), 100, 'r', 'filled');
    ztop(i) = max(partsW(:))*1.5;
    clims = caxis;
    
    if(i > 1)
        mm = measurement;
        mm.ful = [mm.pos(1); min(positionsyy); ztop(i)];
        mm.fbl = [mm.pos(1); min(positionsyy); -ztop(i)/4];
        mm.fbr = [mm.pos(1); max(positionsyy); -ztop(i)/4];
        mm.fur = [mm.pos(1); max(positionsyy); ztop(i)];
        hframe = drawUSFrame(mm, params, []);
        alpha(hframe.usFrame, 0.2);
        set(hframe.usFrame, 'EdgeColor', 'none', 'LineStyle', 'none');
    end
    caxis([clims(1) clims(2)]);
    
    
    set(gca, 'FontSize', 12, 'FontName', 'Times New Roman');
    xlim([min(positionsxx) max(positionsxx)]);
    ylim([min(positionsyy) max(positionsyy)]);
    zlim([0 ztop(i)]);
    daspect([1 1 zaspects(i)])
    rotate(gca, [0 0 1], 45);
    [vaz vel] = view;
    view([vaz+180 vel+15]);
    %view(2);
    %particles = particlesBackup;
    %t = t+0.5;
end
measurement = generateRandomMeasurement(xs, u, ts(2), params);
mm = measurement;
mm.ful = [mm.pos(1); min(positionsyy); ztop(1)];
mm.fbl = [mm.pos(1); min(positionsyy); -ztop(1)/4];
mm.fbr = [mm.pos(1); max(positionsyy); -ztop(1)/4];
mm.fur = [mm.pos(1); max(positionsyy); ztop(1)];
subplot(1,3,1);
hframe = drawUSFrame(mm, params, []);
alpha(hframe.usFrame, 0.2);
set(hframe.usFrame, 'EdgeColor', 'none', 'LineStyle', 'none');

set(figHandle, 'Position', [100 100 3000 500]);

basePath = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Papers\NeedleEstimationJournal\Figures\';

path = strcat(basePath, 'measurement_prob1.png');
%export_fig('-transparent', path);

% for x=center(1)-gridSz/2:gridSz/nParticlesPerDim:center(1)+gridSz/2
%     for y=center(2)-gridSz/2:gridSz/nParticlesPerDim:center(2)+gridSz/2
%         particle.pos = [x; y; 0];
%         particle.q = xs{1}.q;
%         particle.rho = 60;
%         particle.w = 1;
%         particles = vertcat(particles, particle);
%     end
% end


