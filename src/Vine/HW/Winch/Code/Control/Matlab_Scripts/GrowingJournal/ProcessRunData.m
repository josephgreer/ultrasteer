clear; clc; close all;

% fname = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Data/GrowingVine/GrowToLight_OpenLoop.mat';
% tstart = 175;
% tend = 250;

fname = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/GrowToLight1.mat';
lengths = load('~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/GrowToLight1_Lengths.mat');
lengths = lengths.lengths;
tstart = 300;
tend = 415;

lengths(:,2) = lengths(:,2)/max(lengths(:,2))*49*0.0254;

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

% t0 = min(find(time > tstart));
% te = max(find(time < tend));
t0 = 3001;
te = 4149;
lengths = vertcat([linspace(min(lengths(:,1))-(max(lengths(:,1))-min(lengths(:,1)))/3,min(lengths(:,1)),10).' linspace(0,0.03,10).'],lengths);
lengths(11,:) = [];
times = linspace(min(lengths(:,1)),max(lengths(:,1)),te-t0+1).';
lengths = interp1q(lengths(:,1),lengths(:,2),times);
figure;
plot(cumsum(data(:,11)));
figure;
plot(time, data(:,12));
ylim([-180 180]);

h = figure;
set(h, 'Position', [0 0 320 320]);
subplot(2,1,1);
plot(lengths,6*6.895*data(t0:te,1),'LineWidth',4)
hold on;
plot(lengths, 6*6.895*data(t0:te,2),'LineWidth',4);
plot(lengths, 6*6.895*data(t0:te,3),'LineWidth',4);
legend('p_1', 'p_2', 'p_3','Orientation','horizontal'); 
xlabel('Length (m)');
ylabel('Pressure (kPa)');
xlim([0 max(lengths)]);
ylim([0 12]);
set(gca,'FontSize',12, 'FontName', 'Times New Roman');
grid on;

subplot(2,1,2);
hold on;
plot(lengths,data(t0:te,5)-320,'LineWidth',4);
plot(lengths,data(t0:te,6)-240,'Linewidth',4);
xlim([0 max(lengths)]);
xlabel('Length (m)');
ylabel('Pixel Error');
legend('x Error', 'y Error');
set(gca,'FontSize',12, 'FontName', 'Times New Roman');
box on;
grid on;
tightfig;

