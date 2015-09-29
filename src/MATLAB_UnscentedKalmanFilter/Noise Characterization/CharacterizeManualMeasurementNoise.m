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
Nscans = 8;
Nsteps = 2;
scans = 1:Nscans;
steps = 1:Nsteps;
basedir = 'C:\Troy\Data\2015-09-28 Manual Scan Measurement Noise\';
utilpath = '..\Matlab Model\Utility Functions';
addpath(utilpath);

%% Load and process data
for step = steps
    % Load data
    for scan = scans
        file = sprintf('step_%d_measurement_%d.m',step-1,scan-1);
        path = fullfile(basedir,file);
        temp = importdata(path, ' ', 2);
        Z{step}(:,:,scan) = temp.data;
    end 
    % Find mean, covariance and deviation values for the current step
    [mu{step},P{step},E{step}] = meancov(Z{step});    
end

% Find combined covariance
Eall = reshape([E{:}],6,1,[]);
Pall = zeros(6,6);
for i = 1:Nsteps*Nscans
    Pall = Pall + (1/Nsteps/Nscans)*(Eall(:,:,i)*Eall(:,:,i)');
end