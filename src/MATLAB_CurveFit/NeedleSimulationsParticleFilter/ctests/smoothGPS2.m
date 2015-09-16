clear; clc; close all;

addpath('../../LabelData');
basePath = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\8_24_15\Trial2\Insertion\scan';

fgps = fopen(strcat(basePath,'.gps2'));
header = ReadHeader(fgps);

pos = zeros(header.nframes, 3);
for i=1:header.nframes
    currgps = RPreadgps(fgps, header);
    
    pos(i,:) = currgps.pos';
end

fclose all;

scatter(pos(:,1), pos(:,2), pos(:,3));
daspect([1 1 1]);

[b a] = butter(15, 0.1, 'low');

for i=1:3
    pos(:,i) = filtfilt(b, a, pos(:,i));
end
figure;
scatter(pos(:,1), pos(:,2), pos(:,3));
daspect([1 1 1])

save(strcat(basePath, 'smoothed.dat'), 'pos', '-ascii'); 