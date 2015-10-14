% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
% Troy Adebar
% Stanford University
% October 2, 2015
%
% This script analyzes data saved by Teleoperation2DWidget in the
% RTUltrasteer project, in order to characterize the accuracy of
% closed-loop needle steering
% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
%% Initialize
clc; clear all; close all;
basedir = 'C:\Troy\Data\2015-10-09 Closed-Loop Steering Tests\3'
utilpath = '..\Matlab Model\Utility Functions';
addpath(utilpath);

%% Load and process data
file = 'xest.m';
path = fullfile(basedir,file);
temp = importdata(path, ' ', 2);
xest = temp.data;
xest = reshape(xest,4,4,[]);

file = 'xact.m';
path = fullfile(basedir,file);
temp = importdata(path, ' ', 2);
xact = temp.data;
xact = reshape(xact,4,4,[]);

file = 't.m';
path = fullfile(basedir,file);
temp = importdata(path, ' ', 2);
t = temp.data;
t = reshape(t,3,1,[]);

file = 'z.m';
path = fullfile(basedir,file);
temp = importdata(path, ' ', 2);
z = temp.data;
z = reshape(z,4,4,[]);

file = 'u.m';
path = fullfile(basedir,file);
temp = importdata(path, ' ', 2);
u = temp.data;
u = reshape(u,3,1,[]);

%% Remove non-measurements
for i = 1:length(z)
    if(sum(sum(z(:,:,i))) == 0)
        z(:,:,i) = nan;
    end
end

%% Measure estimator error
for i = 1:length(xact)
    estErr(:,:,i) = xact(1:3,4,i)-xest(1:3,4,i);
    estErrNorm(i) = norm(estErr(:,:,i),2);
end

%% Plot results
robotKalmanPlots(false, false, true, t, xact, xest, z, u);

target_error = norm(t(:,:,end)-xact(1:3,4,end))
mean_est_error = mean(estErrNorm)