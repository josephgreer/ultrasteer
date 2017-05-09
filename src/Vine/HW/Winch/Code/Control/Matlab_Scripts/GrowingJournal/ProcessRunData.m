clear; clc; close all;

% fname = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/GrowingVine/GrowToLight_OpenLoop.mat';
% tstart = 175;
% tend = 250;

fname = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/GrowingVine/GrowToLight1.mat';
tstart = 300;
tend = 415;

data = load(fname,'-ascii');


checkColumns = [1,2,3,6,11,12];

badIdxs = [];
for j=1:length(checkColumns)
    diffs = abs(data(2:end,j)-data(1:end-1,j));%max([abs(data(4:end,j)-data(1:end-3,j)),abs(data(4:end,j)-data(2:end-2,j)),abs(data(4:end,j)-data(3:end-1,j))],[],2);
    diffThresh = median(diffs);
    currBad = find(diffs > 1000*diffThresh);
    badIdxs = union(badIdxs,currBad);
end

for i=1:length(badIdxs)
    fixIdx = badIdxs(i)-1;
    while(~isempty(find(badIdxs == fixIdx)))
        fixIdx = fixIdx-1;
    end
    data(badIdxs(i),:) = data(fixIdx,:);
end

[B,A] = butter(2, 0.01);
data(:,1) = filtfilt(B,A,data(:,1));
data(:,2) = filtfilt(B,A,data(:,2));
data(:,3) = filtfilt(B,A,data(:,3));
data(:,5) = filtfilt(B,A,data(:,5));
data(:,6) = filtfilt(B,A,data(:,6));

time = cumsum(0.1*ones(size(data,1),1));

t0 = min(find(time > tstart));
te = max(find(time < tend));
figure;
plot(cumsum(data(:,11)));
figure;
plot(time, data(:,12));
ylim([-180 180]);

figure;
subplot(2,1,1);
plot(time(t0:te),6*data(t0:te,1),'LineWidth',2)
hold on;
plot(time(t0:te), 6*data(t0:te,2),'LineWidth',2);
plot(time(t0:te), 6*data(t0:te,3),'LineWidth',2);
legend('sPAM 1', 'sPAM 2', 'sPAM 3'); 
xlabel('Time (s)');
ylabel('Pressure (PSI)');
xlim([tstart tend]);

subplot(2,1,2);
hold on;
plot(time(t0:te),data(t0:te,5)-320,'LineWidth',2);
plot(time(t0:te),data(t0:te,6)-240,'Linewidth',2);
xlim([tstart tend]);
xlabel('Time (s)');
ylabel('Pixel Error');
legend('x Error', 'y Error');
box on;
tightfig;

