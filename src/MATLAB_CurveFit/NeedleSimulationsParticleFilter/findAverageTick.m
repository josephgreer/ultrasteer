clear; clc; close all;

addpath('../../LabelData');
basebasePath = 'C:\Joey\Data\9_18_15\3mm\Trial1';
basePath = strcat(basebasePath,'\scan');

speed = 3; % mm/s
% 
% fgps = fopen(strcat(basePath,'.gps2'));
% header = ReadHeader(fgps);
% 
% pos = zeros(header.nframes, 3);
% for i=1:header.nframes
%     currgps = RPreadgps(fgps, header);
%     
%     pos(i,:) = currgps.pos';
% end

pos = load(strcat(basebasePath, '\PreInsertionGPS.mat'), '-ascii');
pos = pos(1:end-1,:);

fclose all;

scatter3(pos(:,1), pos(:,2), pos(:,3));
daspect([1 1 1]);

shift = 2;
posShift = pos(shift:end,:);
deltas = posShift-pos(1:end-shift+1, :);
deltas = sqrt(sum(deltas.^2,2));
figure;
plot(deltas);
aveNorm = mean(deltas);
aveTimeMs = 1e3*aveNorm/(speed*(shift-1))

%save(strcat(basePath, 'smoothed.dat'), 'pos', '-ascii'); 