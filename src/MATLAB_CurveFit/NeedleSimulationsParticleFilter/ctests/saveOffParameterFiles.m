% this file finds all results and puts them into a unified folder system.

clear; clc; close all;

baseDir = 'C:\Joey\Data\';
outBaseDir = 'C:\Joey\ultrasteer\data\NeedlePFParams\';

excludeDir = {};

data = vertcat(rdir(strcat(baseDir,'**\params.dat')), rdir(strcat(baseDir,'**\params_marg.dat')));

for i=1:length(data)
    excluded = 0;
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
    
    [currOutDir,~] = regexp(data(i).name, '.*(\d+\_\d+\_\d+\\\d+mm\\Trial\d+).*', 'tokens', 'match'); 
    if(isempty(currOutDir))
        continue;
    end
    currOutDir = cell2mat(currOutDir{1});
    
    fullOutDir = strcat(outBaseDir, currOutDir, '\');
    if(~exist(fullOutDir, 'dir'))
        mkdir(fullOutDir);
    end
    
    copyfile(data(i).name, fullOutDir, 'f');
end