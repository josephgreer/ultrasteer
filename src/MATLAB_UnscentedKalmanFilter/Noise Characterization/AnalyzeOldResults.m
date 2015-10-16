% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
% Troy Adebar
% Stanford University
% October 2, 2015
%
% This script analyzes data saved during the IROS testing, in order to
% determine the mean and covariance of process and measurement noise for
% the setup in that testing.
% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
%% Initialize
clc; clear all;
%% Load data
load('Results\OldProcessNoise.mat');
load('Results\OldMeasurementNoise.mat');
%% Process
% Convert vector representations to matrix representations
for i = 1:length(W)/2;
    WT(:,:,i) = state2trans(W(:,i));
end
for i = 1:length(V);
    VT(:,:,i) = state2trans(V(:,i));
end

[muW, Q, ~] = meancov(WT)
[muV, R, ~] = meancov(VT)
