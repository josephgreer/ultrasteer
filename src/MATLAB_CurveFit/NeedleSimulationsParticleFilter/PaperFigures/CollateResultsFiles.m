% this file finds all results and puts them into a unified folder system.

clear; clc; close all;

addpath('../ctests/');

baseDir = 'C:\Joey\Data\NeedleScan\';
outBaseDir = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\JournalResults\';

excludeDir = {'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\9_18_15\Results\9_18_15\05mm',...
    'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\JournalResults'};

data = rdir(strcat(baseDir,'**\*EstimatedPos.dat'))

speedLabels = {'05mm', '1mm', '2mm', '3mm'};
speeds = [0.5 1 2 3];

showPlots = 0;

defaultPreamble = 'FullState';

results = {[],[],[],[]};
for i=1:length(data)
    excluded = 0;
    close all;
    currDir = data(i).name;
    idxs = strfind(currDir, '\');
    currDir = currDir(1:idxs(end));
    
    for jj=1:length(excludeDir)
       if(length(findstr(currDir, excludeDir{jj})) > 0)
           excluded = 1;
           break;
       end
    end
    
    if(excluded)
        continue;
    end
    
    [preamble, ~] = regexp(data(i).name, '.*\\(\w+)EstimatedPos\.dat', 'tokens', 'match');
    if(~isempty(preamble))
        preamble = char(cell2mat(preamble{1}));
    else
        preamble = '';
    end
    
    [currOutDir,~] = regexp(data(i).name, '.*(\d+\_\d+\_\d+\\\d+mm\\Trial\d+).*', 'tokens', 'match'); 
    currOutDir = cell2mat(currOutDir{1});
    
    estBasePath = strcat(currDir,preamble,'Estimated');
    truthBasePath = strcat(currDir,preamble,'GroundTruth');
    
    estPos = strcat(estBasePath, 'Pos.dat');
    truePos = strcat(truthBasePath, 'Pos.dat');
    
    estRs = strcat(estBasePath, 'Rs.dat');
    trueRs = strcat(truthBasePath, 'Rs.dat');
    
    estRhos = strcat(estBasePath, 'Rho.dat');
    trueRhos = strcat(truthBasePath, 'Rho.dat');
    
    fullOutDir = strcat(outBaseDir, currOutDir, '\');
    if(~exist(fullOutDir, 'dir'))
        mkdir(fullOutDir);
    end
    
    if(length(preamble) == 0)
        preamble = defaultPreamble;
    end
    
    fullOutDirPath = strcat(fullOutDir, preamble, 'EstimatedPos.dat');
    copyfile(estPos, fullOutDirPath, 'f');
    fullOutDirPath = strcat(fullOutDir, preamble, 'EstimatedRs.dat');
    copyfile(estRs, fullOutDirPath, 'f');
    fullOutDirPath = strcat(fullOutDir, preamble, 'EstimatedRhos.dat');
    copyfile(estRhos, fullOutDirPath, 'f');
    
    
    fullOutDirPath = strcat(fullOutDir, preamble, 'GroundTruthPos.dat');
    copyfile(truePos, fullOutDirPath, 'f');
    fullOutDirPath = strcat(fullOutDir, preamble, 'GroundTruthRs.dat');
    copyfile(trueRs, fullOutDirPath, 'f');
    fullOutDirPath = strcat(fullOutDir, preamble, 'GroundTruthRhos.dat');
    copyfile(trueRhos, fullOutDirPath, 'f');
end