% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
% Troy Adebar
% Stanford University
% October 2, 2015
%
% This script analyzes data saved by Teleoperation2DWidget in the
% RTUltrasteer project, in order to characterize the process noise for
% motion of the needle tip
% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
%% Initialize
clc; clear all; close all;
% Nscans = 5;
% Nsteps = {2,8,5,2,7,4,6,6};
% Nruns = 8;
% scans = 1:Nscans;
% runs = 1:Nruns;
basedir = 'C:\Troy\Data\'
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

% %% Correct initial registration error between estimate and in-needle GPS
% Te = xact(:,:,3)^-1*xest(:,:,3);
% for i = 1:length(xact)
%     xact(:,:,i) = xact(:,:,i)*Te;
% end

%% Remove non-measurements
for i = 1:length(z)
    if(sum(sum(z(:,:,i))) == 0)
        z(:,:,i) = nan;
    end
end
% 
% for i = length(xest):-1:1
%     if(u(3,:,i) == 0)
%         xact(:,:,i) = [];
%         xest(:,:,i) = [];
%         u(:,:,i) = [];
%         z(:,:,i) = [];
%     end
% end


%% Plot results
robotKalmanPlots(true, false, true, t, xact, xest, z, u);

error = norm(t(:,:,1)-xact(1:3,4,end))
