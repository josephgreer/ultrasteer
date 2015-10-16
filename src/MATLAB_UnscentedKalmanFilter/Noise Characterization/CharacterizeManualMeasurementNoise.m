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
basedir = 'F:\Dropbox\Troy Adebar Research Folder\1 - Kalman Filter\Data\2015-09-29 Manual Scan Measurement Noise';
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

%% Save matrix of errors
for i = 1:N
    V(:,i) = Eall(1:3,:,i);
end
mu_ve = mean(V,2);
Re = cov(V');
save('Results\MeasurementNoise','V','mu_ve','Re');

%% Visualize errors

% Errors
figure(2); clf; hold on;
plot3(squeeze(Eall(1,1,:)),squeeze(Eall(2,1,:)),squeeze(Eall(3,1,:)),'k.');
set(gcf,'color','w');
axis equal; grid on; box on;
xlabel('x direction (mm)'); 
ylabel('y direction (mm)'); 
zlabel('z direction (mm)');
view(50,20);
axis([-6 6 -6 6 -6 6]);
set(findall(gcf,'-property','FontSize'),'FontSize',14,...
    'fontName','Times New Roman')