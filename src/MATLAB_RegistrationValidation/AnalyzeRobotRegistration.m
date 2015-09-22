% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
% Troy Adebar
% Stanford University
% September 22, 2015
%
% This script analyzes data saved by EMCalibrationWidget in the
% RTUltrasteer project, in order to determine the accuracy of robot to EM
% registration.
% -------------------------------------------------------------------------
% -------------------------------------------------------------------------
%% Initialize
clc; clear all; close all;

%% Load data
basedir = 'C:\Troy\Data\2015-09-22 Robot Registration Validation\2\';

temp = importdata(fullfile(basedir,'EMCal.m'), ' ', 2);
T = temp.data;

temp = importdata(fullfile(basedir,'fiducialMeasurements 0.m'), ' ', 2);
p1 = temp.data';

temp = importdata(fullfile(basedir,'fiducialMeasurements 1.m'), ' ', 2);
p2 = temp.data';

temp = importdata(fullfile(basedir,'fiducialMeasurements 2.m'), ' ', 2);
p3 = temp.data';

temp = importdata(fullfile(basedir,'fiducialMeasurements 3.m'), ' ', 2);
p4 = temp.data';

temp = importdata(fullfile(basedir,'fiducialsInFrobot.m'), ' ', 2);
fiducialsInFrobot = temp.data;

temp = importdata(fullfile(basedir,'fiducialsInFref.m'), ' ', 2);
fiducialsInFref = temp.data;

%% Solve robot registration

[d, z, trans] = procrustes(fiducialsInFrobot',fiducialsInFref',...
    'Scaling', false, 'Reflection', false)

T_procrustes = [ trans.T' trans.c(1,:)'; [0 0 0 1] ]
T
 
e = z-fiducialsInFrobot';
FRE_avg = mean( sum(e.^2,2).^0.5 )

