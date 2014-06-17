% Author: Ann Majewicz
% Date: 7/27/2010
% Needle Steering Project
% Animal Study - Determining insertion speed
% Johns Hopkins University
%-------------------------------------------------------------------------
%

close all;
clear all;
clc;

Pfiles = dir('FORCE DATA/P*');
Kfiles = dir('FORCE DATA/K*');
Lfiles = dir('FORCE DATA/L*');


%% Prostate Data


for i = 1:length(Pfiles)
    data = load(strcat('FORCE DATA/P',num2str(i),'/QD_CH12.txt'));
    figure
    plot(data(:,2));
    parameters = fopen(strcat('FORCE DATA/P',num2str(i),'/params.txt'));
    InputText=textscan(parameters,'%s','delimiter','\n'); % Read lines
    param = InputText{1};
    disp(param)
    
end


%% Kidney Data


for i = 1:length(Kfiles)
    data = load(strcat('FORCE DATA/K',num2str(i),'/QD_CH12.txt'));
    figure
    plot(data(:,2));
    parameters = fopen(strcat('FORCE DATA/K',num2str(i),'/params.txt'));
    InputText=textscan(parameters,'%s','delimiter','\n'); % Read lines
    param = InputText{1};
    disp(param)

end


%% Liver Data

for i = 1:length(Lfiles)
    data = load(strcat('FORCE DATA/L',num2str(i),'/QD_CH12.txt'));
    figure
    plot(data(:,2));
    parameters = fopen(strcat('FORCE DATA/L',num2str(i),'/params.txt'));
    InputText=textscan(parameters,'%s','delimiter','\n'); % Read lines
    param = InputText{1};
    disp(param)
    
end