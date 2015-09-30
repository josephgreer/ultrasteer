% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
% Troy Adebar
% Stanford University
% September 28, 2015
%
% This script analyzes data saved by Teleoperation2DWidget in the
% RTUltrasteer project, in order to characterize the measurement noise for
% manual selection of the needle tip
% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
%% Initialize
clc; clear all; close all;
Nscans = 5;
Nsteps = {2,8,5,2,7,4,6,6};
Nruns = 8;
scans = 1:Nscans;
runs = 1:Nruns;
basedir = 'C:\Troy\Data\2015-09-29 Manual Scan Measurement Noise\';
utilpath = '..\Matlab Model\Utility Functions';
addpath(utilpath);

%% Load and process data
i = 1;
for run = runs
    for step = 1:Nsteps{run}
        % Load data
        for scan = scans
            fold = sprintf('Run%d',run);
            file = sprintf('step_%d_measurement_%d.m',step-1,scan-1);
            path = fullfile(basedir,fold,file);
            temp = importdata(path, ' ', 2);
            Z{i}(:,:,scan) = temp.data;
        end
        % Find mean, covariance and deviation values for the current step
        [mu{i},P{i},E{i}] = meancov(Z{i});
        i = i+1;
    end
end

% Find combined covariance
Zall = reshape([Z{:}],4,4,[])
Eall = reshape([E{:}],6,1,[]);
muall = reshape([mu{:}],4,4,[]);
Pall = zeros(6,6);
for j = 1:length(Eall)
    Pall = Pall + (1/length(Eall))*(Eall(:,:,j)*Eall(:,:,j)');
end
display(Pall)
N = length(Eall);
display(N)

%% Visualize measurements, errors

% Measurements
figure(1); clf; hold on;
plot3(squeeze(Zall(1,4,:)),squeeze(Zall(2,4,:)),squeeze(Zall(3,4,:)),'ro');
plot3(squeeze(muall(1,4,:)),squeeze(muall(2,4,:)),squeeze(muall(3,4,:)),'bx');
set(gcf,'color','w');
axis equal; grid on;
xlabel('x'); ylabel('y'); zlabel('z');

% Errors
figure(2); clf; hold on;
plot3(squeeze(Eall(1,1,:)),squeeze(Eall(2,1,:)),squeeze(Eall(3,1,:)),'g^');
set(gcf,'color','w');
axis equal; grid on;
xlabel('x'); ylabel('y'); zlabel('z');