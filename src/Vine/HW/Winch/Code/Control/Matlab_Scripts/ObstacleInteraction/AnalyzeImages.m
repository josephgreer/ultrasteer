clear; clc; close all;

w = warning('off','all');

runs = linspace(1,18,18);
baseDir = '/Users/Joey/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/PegForest/3/Images/%02d_*.jpg';

for run=runs
    currDir = sprintf(baseDir,run);
    files = rdir(currDir);
    
    if(exist(sprintf('%02d.mat', run), 'file'))
        continue;
    end
    
    pts = {};
    for i=1:length(files)
        im = imread(files(i).name);
        imshow(im);
        [x, y] = ginput;
        pts{i} = horzcat(x,y);
    end
    
    save(sprintf('%02d.mat', run), 'pts');
    
end