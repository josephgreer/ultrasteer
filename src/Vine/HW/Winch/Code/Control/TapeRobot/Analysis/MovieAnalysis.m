clear; clc; close all;

fid = fopen('GrowToLight_2.txt');

data = cell2mat(textscan(fid, '%f %f %f %f %f %f'));

idx = min(find(data(:,4) > 0))
data = data(359:end,:);

data(:,1) = ((data(:,1)-data(1,1))*1e-3)/64;
idx = min(find(data(:,1) > 23.13));
data = data(1:idx,:);

plot(data(:,1));
figure;
plot(data(:,1),data(:,end));
figure;
hold on;
X = [min(data(:,1)); max(data(:,1)); max(data(:,1)); min(data(:,1))];
Y = [50; 50; -50; -50];
patch = fill(X,Y, 'r');
patch.FaceAlpha = 0.1;
patch.LineStyle = 'None';
Y = [50; 50; 320; 320];
patch = fill(X,Y, 'b');
patch.FaceAlpha = 0.1;
patch.LineStyle = 'None';
Y = [-50; -50; -320; -320];
patch = fill(X,Y, 'g');
patch.FaceAlpha = 0.1;
patch.LineStyle = 'None';
% plot(data(:,1),data(:,4)-320,'k','LineWidth',2);
d1 = designfilt('lowpassiir','FilterOrder',12, ...
    'HalfPowerFrequency',0.05,'DesignMethod','butter');
dataSmoothed = filtfilt(d1,double(data(:,4)-320));
plot(data(:,1),dataSmoothed,'r','LineWidth',2);
ylim([-300 300]);
xlim([min(data(:,1)) max(data(:,1))]);
xlabel('Time (s)');
ylabel('Horizontal Light Location (px)');
view(90,90);
% export_fig -transparent '~/Dropbox' (Stanford CHARM Lab)'/Science Submission/ErrorPlot.pdf';
box on;
grid on;
print(gcf, '-dpdf', '~/Dropbox (Stanford CHARM Lab)/Science Submission/ErrorPlot.pdf');
figure;
plot(data(:,1),data(:,2));
hold on;
plot(data(:,1),data(:,3));
ylim([-0.5 1.5]);

fclose all;