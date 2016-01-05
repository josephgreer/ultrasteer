clear; clc; close all;

addpath('../../LabelData');
basePath = 'C:\Joey\Data\12_16_15\05mm\Trial6\scan';

fgps = fopen(strcat(basePath,'.gps2'));
header = ReadHeader(fgps);

pos = zeros(header.nframes, 3);
for i=1:header.nframes
    currgps = RPreadgps(fgps, header);
    
    pos(i,:) = currgps.pos';
end

fclose all;

scatter3(pos(:,1), pos(:,2), pos(:,3));
daspect([1 1 1]);

[b a] = butter(15, 0.1, 'low');

for i=1:3
    pos(:,i) = filtfilt(b, a, pos(:,i));
end
figure;
scatter3(pos(:,1), pos(:,2), pos(:,3));
daspect([1 1 1])

save(strcat(basePath, 'smoothed.dat'), 'pos', '-ascii'); 