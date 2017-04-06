clear; clc; close all;

points = load('success_left_first.txt');
points = points(1:end-1,:);

pos = points(:,1:2);
pos = pos-repmat(pos(1,:),size(pos,1),1);
pos = pos(1:600,:);
npoints = size(pos, 1);

fps = 9;

time = [1:npoints]/9;

% patch up mess up frames 136-180
jump = 50;
beginIdx = 148; endIdx = 176;
begin = pos(beginIdx,:);
ed = pos(endIdx,:);
fixX = linspace(begin(1),ed(1), endIdx-beginIdx+1).';
fixY = linspace(begin(2),ed(2), endIdx-beginIdx+1).';
pos(beginIdx:endIdx,:) = [fixX fixY];

d1 = designfilt('lowpassiir','FilterOrder',12, ...
    'HalfPowerFrequency',0.05,'DesignMethod','butter');
lowPassPos = zeros(size(pos));
lowPassPos(:,1) = filtfilt(d1,pos(:,1));
lowPassPos(:,2) = filtfilt(d1,pos(:,2));

deltas = (pos(jump:end,:)-pos(1:end-(jump-1),:))/(jump-1);

obs = points(:,3:4);

figure(10);
subplot(3,1,3);
box on;
scatter(pos(:,1)/10,pos(:,2)/10);
box on;
legend('head trajectory');
ylabel('cm');
xlabel('cm');
daspect([1 1 1]);

% segment

figure;
scatter(lowPassPos(:,1),lowPassPos(:,2));
legend('lp robot head position');
daspect([1 1 1]);

figure;
hold on;
plot(time,pos(:,1));
plot(time,pos(:,2));
plot(time,lowPassPos(:,1));
plot(time,lowPassPos(:,2));
legend('x pos', 'y pos', 'lp x', 'lp y');

figure;
hold on;
nvel = size(deltas,1);
plot([1:nvel]/fps, deltas(:,1));
plot([1:nvel]/fps, deltas(:,2));
legend('x vel', 'y vel');

figure(10);
subplot(3,1,1);
headings = rad2deg(atan2(deltas(:,2),deltas(:,1)));
box on;
plot([1:nvel]/fps, headings);

segments = [1 186 319 520 600];
seg1 = pos(segments(1):segments(2), :);
seg2 = pos(segments(2)+1:segments(3), :);
seg3 = pos(segments(3)+1:segments(4), :);
seg4 = pos(segments(4)+1:segments(5), :);
segs = {seg1,seg2,seg3,seg4};

deltaSegs = [];

avgThetas = [];

for i=1:length(segs)
    lowPassSeg = zeros(size(segs{i}));
    lowPassSeg(:,1) = filtfilt(d1,segs{i}(:,1));
    lowPassSeg(:,2) = filtfilt(d1,segs{i}(:,2));
    
    deltaSeg = lowPassSeg(jump:end,:)-lowPassSeg(1:end-(jump-1),:);
    deltaSeg = vertcat(deltaSeg,repmat(deltaSeg(end,:),jump-1,1));
    deltaSegs = vertcat(deltaSegs, deltaSeg);
end


for i=1:length(segments)-1    
    deltaSeg = lowPassPos(segments(i+1),:)-lowPassPos(segments(i),:);
    avgTheta = rad2deg(atan2(deltaSeg(2),deltaSeg(1)));
    avgThetas = vertcat(avgThetas,repmat(avgTheta,segments(i+1)-segments(i)+1,1));
end

figure(10);
subplot(3,1,1);
box on;
plot([1:length(headings)]/fps,headings);
hold on;
avgThetas = avgThetas(1:length(headings));
plot([1:length(avgThetas)]/fps,avgThetas);
xlabel('time (s)');
legend('tip orientation', 'body orientation');
ylabel('orientation (degrees)');
xlim([0 length(avgThetas)/fps]);

figure(10);
subplot(3,1,2);
box on;
jump = 10;
deltas = pos(jump:end,:)-pos(1:end-(jump-1),:);
deltas = deltas/(jump-1);
plot([1:(npoints-jump+1)]/fps,sqrt(sum(deltas.^2,2)))
hold on;
xlim([0 (npoints-jump+1)/fps]);
ylim([0 2.5]);
xlabel('time (s)');
ylabel('cm/second');
legend('tip speed','maximum tip velocity','commanded pressure');

actuatorIdxs = [1 15 100 148 186 280 318 409 465 502];
actuatorTimes = actuatorIdxs/fps;

commandedVels = [2.5 0.603 0.603 2.5 0.603 2.5 0.603 0.603 2.5];
commandedPressures = [2.4 6 6 2.4 6 2.4 6 6 2.4];
cPres = [];
cVels = [];
for i=1:length(actuatorIdxs)-1
    curLen = actuatorIdxs(i+1)-actuatorIdxs(i);
    cVels = vertcat(cVels,ones(curLen,1)*commandedVels(i));
    plot(actuatorTimes(i)*ones(2,1), [0 2.5], 'k','LineWidth',2);
end
lastLen = npoints-actuatorIdxs(end)+1;
cVels = vertcat(cVels, ones(lastLen,1)*0.6);
cPres = vertcat(cPres, ones(lastLen,1)*2.4);

hold on;
plot([1:npoints-(jump-1)]/fps,cVels(1:npoints-jump+1));

subplot(3,1,1);
ylim([-50 50]);
for i=1:length(actuatorIdxs)-1
    plot(actuatorTimes(i)*ones(2,1), [-50 50], 'k','LineWidth',2);
end

% hold on;
% plotyy([1:npoints]/fps,cVels,1:npoints,cPres);


