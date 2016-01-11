% this file finds all results and puts them into a unified folder system.

clear; clc; close all;

addpath('../ctests/');

baseDir = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\';
outBaseDir = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\JournalResults\';

data = rdir(strcat(baseDir,'**\*EstimatedPos.dat'))

speedLabels = {'05mm', '1mm', '2mm', '3mm'};
speeds = [0.5 1 2 3];

showPlots = 0;

results = {[],[],[],[]};
for i=1:length(data)
    close all;
    currDir = data(i).name;
    idxs = strfind(currDir, '\');
    currDir = currDir(1:idxs(end));
    
    [preamble, ~] = regexp(data(i).name, '.*\\(\w+)EstimatedPos\.dat', 'tokens', 'match');
    preamble = char(cell2mat(preamble{1}));
    
    currOutDir = currDir(idxs(end-4)+1:idxs(end-1));
    
    estBasePath = strcat(currDir,preamble,'Estimated');
    truthBasePath = strcat(currDir,preamble,'GroundTruth');
    
    estPos = strcat(estBasePath, 'Pos.dat');
    truePos = strcat(truthBasePath, 'Pos.dat');
    
    estRs = strcat(estBasePath, 'Rs.dat');
    trueRs = strcat(truthBasePath, 'Rs.dat');
    
    estRhos = strcat(estBasePath, 'Rho.dat');
    trueRhos = strcat(truthBasePath, 'Rho.dat');
    
    fullOutDir = strcat(outBaseDir, currOutDir);
    if(~exist(fullOutDir, 'dir'))
        mkdir(fullOutDir);
    end
    
    copyfile(estPos, fullOutDir, 'f');
    copyfile(estRs, fullOutDir, 'f');
    copyfile(estRhos, fullOutDir, 'f');
    copyfile(truePos, fullOutDir, 'f');
    copyfile(trueRs, fullOutDir, 'f');
    copyfile(trueRhos, fullOutDir, 'f');
end