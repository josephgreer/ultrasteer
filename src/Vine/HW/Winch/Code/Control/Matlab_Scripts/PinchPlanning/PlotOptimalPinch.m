clear; clc; close all;

resFile = rdir('Results/result*.mat');
nFiles = length(resFile);

stds = zeros(nFiles,1);
turnMags = zeros(nFiles,1);
turnSpacings = zeros(nFiles,1);

for i=1:nFiles
    load(resFile(i).name);
    
    stds(i) = std(currResult.angleErrors);
    turnMags(i) = currResult.turnMag;
    turnSpacings(i) = currResult.turnSpacing;
end

scatter(turnMags,turnSpacings, 100, stds);
xlabel('Turn Mags');
ylabel('Turn Spacing');
colorbar;

[min,minI] = min(stds);


stds(turnMags == 0)