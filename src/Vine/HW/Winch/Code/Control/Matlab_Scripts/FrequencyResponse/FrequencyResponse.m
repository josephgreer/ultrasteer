clear; clc; close all;

load -ascii pos.mat

begin = 0.6;
pos(:,1) = (pos(:,1)-pos(1,1))*1e-3;
plot(pos(:,1), pos(:,2));
hold on;
plot(pos(:,1), pos(:,3));
plot(pos(:,1), pos(:,4));

Rs = cell(size(pos,1),1);
for i=1:size(pos,1)
    Rs{i} = reshape(pos(i,5:13),3,3).';
end

points = begin+[0:23:1570];
points = points(1:end-1);
magnitudes = zeros(length(points),1);
magnitudes2 = zeros(length(points),1);
freqs = 0.05+0.03*linspace(0,length(points)-3,length(points)-2);
freqs = [0 0 freqs];

angles = [];
for i=3:length(points)-1
    plot([points(i) points(i)], [0 400], 'r');
    plot([points(i)+20 points(i)+20], [0 400], 'g');
%     
    
    windowPoints = find(points(i)<pos(:,1) & pos(:,1) < points(i)+20);
    [~,startPoint] = min(abs(points(i)-pos(:,1)));
    
    xs = cell2mat(Rs(windowPoints));
    xs = reshape(xs(:,1), 3, length(windowPoints));
    refx = Rs{startPoint}(:,1);%xs(:,round(size(xs,2)/2));
    dotx = refx.'*xs;
    dotx = rad2deg(acos(dotx./(norm(refx)*sqrt(sum(xs.^2,1)))));
    magnitudes(i) = abs(max(dotx)-min(dotx));
    
    poss = pos(windowPoints, 2:4);
    poss = poss-repmat(pos(startPoint,2:4),size(poss,1),1);
    poss = sqrt(sum(poss.^2,2));
    magnitudes2(i) = max(poss);
    
    windowPoints = find(points(i)<pos(:,1) & pos(:,1) < points(i)+20);
    xs = cell2mat(Rs(windowPoints));
    xs = reshape(xs(:,1), 3, length(windowPoints));
    refx = Rs{startPoint}(:,1);
    dotx = refx.'*xs;
    dotx = rad2deg(acos(dotx./(norm(refx)*sqrt(sum(xs.^2,1)))));
    angles = vertcat(angles, [pos(windowPoints,1) dotx.']);
    
end

options = optimoptions('lsqcurvefit','MaxFunEvals', 100000, 'MaxIter', 5000);
F = @(x,xdata)((x(1)*xdata)./(x(4)*xdata.^2+x(3)*xdata+x(2)));
params = lsqcurvefit(F,[1 1 1 1],freqs.',magnitudes,[],[],options);

ylim([75 200]);
figure;
scatter(freqs,magnitudes, 'LineWidth', 2);
hold on;
freqsExtended = linspace(0.05,100,1000);
plot(freqs, F(params,freqs), 'LineWidth', 2);
ylim([3 15]);
xlim([min(freqs) max(freqs)]);
xlabel('Frequency (Hz)');
ylabel('Angular Deflection');
set(gca,'xscale','log');
set(gca,'yscale','log');
box on;

figure;
scatter(freqs,magnitudes2);

phases = zeros(length(freqs),1);

figure;
plot(angles(:,1), angles(:,2));
hold on;
for i=3:length(points)-1
    plot([points(i) points(i)], [0 10], 'r');
    plot([points(i)+20 points(i)+20], [0 10], 'g');
    
    
    windowPoints = find(points(i)<angles(:,1) & angles(:,1) < points(i)+20);
    
    timeStep = mean(angles(2:end,1)-angles(1:end-1,1));
    
    ts = angles(windowPoints,1)-angles(windowPoints(1),1);
    refSignal = 0.5*magnitudes(i)*((sin(2*pi*freqs(i)*ts)+1));
    plot(angles(windowPoints,1), refSignal, 'k--');
    
    [crossCorr, lag] = xcorr(angles(windowPoints,2),refSignal);
  
    [~, stepLag] = max(abs(crossCorr));
    stepLag = lag(stepLag);
    period = 1/freqs(i);
    phases(i) = mod(360*(stepLag*timeStep)/period,360);
    if(abs(360-phases(i)) < abs(phases(i)))
        phases(i) = 360-phases(i);
    end
    phases(i) = -phases(i);
    
    yepyep = 0;
end
figure;
scatter(freqs, phases);