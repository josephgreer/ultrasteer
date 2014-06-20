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
addpath('./Functions');
addpath('./GUIs');
addpath('./Functions/lsge-matlab');
folder = 'C:\Users\charm\Desktop\Needle Steering\06_11_14 Cadaver Study\Data\DYNACT_HEAD_NAT_FILL_HU_NORMAL_[INSPACE3D]_0054';

dbstop in generateSummaryData;


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
disp(['Evaluating Scan :' folder]);
shapeAnalysis = ShapeAnalysisCode(folder,1,[]);                    % 1 is for inVivo.


% -------------------------------------------------------------------------
% Post shape analysis
% -------------------------------------------------------------------------

% Save the shapeAnalysis at the end. 
%save([folder '/shapeAnalysis.mat'],'shapeAnalysis');

% Plot the isosurfaces with the fitted lines/circles. If plotimages == 1.
plotimages = 1;
if plotimages == 1
    for i=1:length(shapeAnalysis)
        if ~isempty(shapeAnalysis(i).line)
            [Xpts,Ypts,Zpts] = Linepts(shapeAnalysis(i).line(1:3),shapeAnalysis(i).line(4:6));
             figure
             isosurface(shapeAnalysis(i).Image)
             hold on
             plot3(Xpts,Ypts,Zpts)
             title(strtrim(folder))
             axis([0 300 0 300 0 200])
        else
             figure
             isosurface(shapeAnalysis(i).Image)
             hold on
             circle3(shapeAnalysis(i).curve(2:4),shapeAnalysis(i).curve(5:7),shapeAnalysis(i).curve(1))
             axis([0 300 0 300 0 200])
             title(strtrim(folder))
        end

    end
end

% Generate Summary data and save
summaryData = generateSummaryData(shapeAnalysis,1,1);
%save([folder '/summaryData.mat'],'summaryData');
%plotSummaryData(summaryData);

