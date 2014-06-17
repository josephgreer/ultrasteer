% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% CT Scan Analysis Code :
%   This script will guide a user through the process of segmenting out
%   needles in CT scans and fitting curves (lines or circles) to those
%   needles. 
% =========================================================================

% -------------------------------------------------------------------------
% Initialization of variables
% -------------------------------------------------------------------------
clear all
close all
clc
addpath('Functions');
addpath('GUIs');
addpath('Functions/lsge-matlab');
folder = [uigetdir('../') '/'];

scans = dir([folder '/Scan*'])

scansDD = [];
scansSD = [];
scansNumber = zeros(length(scans),1);

for i = 1:length(scans)                                                     % Since the scan number may be double digits, the ordering that dir does, messes up the numerical order so it has to be fixed...
    scansNumber(i) = str2num(scans(i).name(5:end));
    if length(scans(i).name) == 5
        scansSD = [scansSD ; scans(i).name];
    else
        scansDD = [scansDD ; scans(i).name];
    end
end

[mS nS] = size(scansSD);
[mD nD] = size(scansDD);

% Need to pad a space on the end of the vector of the scan name for single
% digit scans so that they can be ordered with the others. Now that I think
% about this, I'm not sure if there isnt a better way... need to come back.
scansSDSpace = [];
for i =1:mS
    scansSDSpace(i,:) = [scansSD(i,:),' '];
end


scansNumberSorted = sort(scansNumber);                                     % Sort the numbers


scansOrdered = [scansSDSpace;scansDD];                                     % Order the scans single digit first. 
[mO nO] = size(scansOrdered);                                              % mO gives the number of total scans in the folder. 


% Create the shape analysis sturcture which will save all relevant info.
shapeAnalysis = struct('curve','curveSTDERR','line','lineRESNORM','Image','Data');
%shapeAnalysis = struct([]);
 shapeAnalysis.curve = [];
 shapeAnalysis.curveSTDERR = [];
 shapeAnalysis.line = [];
 shapeAnalysis.lineRESNORM = [];
 shapeAnalysis.Image = [];
 shapeAnalysis.Data = [];


start = 1;
check = what(folder);

% Check if there exists a shapeAnalysis mat file. If not create it, if so,
% load and check length for when to start.
if ~isempty(find(strcmp(check.mat,'shapeAnalysis.mat'),1))
    load([folder '/shapeAnalysis.mat']);
    m = length(shapeAnalysis);
    start = m+1;
    if isempty(shapeAnalysis(m).curve) && isempty(shapeAnalysis(m).line);
        start = m;
    end
end

% -------------------------------------------------------------------------
% Perform shape analysis
% -------------------------------------------------------------------------
for i=start:mO                                                             %Perform shape analysis for all scans that have not been analyzed yet. 
    scanFolder = [folder strtrim(scansOrdered(i,:)) '/'];
    disp(['Evaluating Scan :' strtrim(scansOrdered(i,:)) ]);
    shapeAnalysis(i) = ShapeAnalysisCode(scanFolder,1,[]);                    % 1 is for inVivo. 
end


% -------------------------------------------------------------------------
% Post shape analysis
% -------------------------------------------------------------------------

% Save the shapeAnalysis at the end. 
save([folder '/shapeAnalysis.mat'],'shapeAnalysis');

% Plot the isosurfaces with the fitted lines/circles. If plotimages == 1.
plotimages = 0;
if plotimages == 1
    for i=1:length(shapeAnalysis)
        if ~isempty(shapeAnalysis(i).line)
            [Xpts,Ypts,Zpts] = Linepts(shapeAnalysis(i).line(1:3),shapeAnalysis(i).line(4:6));
             figure
             isosurface(shapeAnalysis(i).Image)
             hold on
             plot3(Xpts,Ypts,Zpts)
             title(strtrim(scansOrdered(i,:)))
             axis([0 300 0 300 0 200])
        else
             figure
             isosurface(shapeAnalysis(i).Image)
             hold on
             circle3(shapeAnalysis(i).curve(2:4),shapeAnalysis(i).curve(5:7),shapeAnalysis(i).curve(1))
             axis([0 300 0 300 0 200])
             title(strtrim(scansOrdered(i,:)))
        end

    end
end

% Generate Summary data and save
summaryData = generateSummaryData(shapeAnalysis,scansNumberSorted);
save([folder '/summaryData.mat'],'summaryData');
%plotSummaryData(summaryData);

