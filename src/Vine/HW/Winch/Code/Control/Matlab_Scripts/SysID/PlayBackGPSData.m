clearvars -except positionFrames; clc; close all;

gpsFile = 'Data/EMTracker/Trial4/scan.gps2';
pressureFile = 'Data/EMTracker/Trial4/Pressure.mat';

load(pressureFile);
addpath('../../../../../../../MATLAB_CurveFit/LabelData/');
addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter');

fgps = fopen(gpsFile);
gpsHeader = ReadHeader(fgps);

range = FindGPSPosRange(fgps, gpsHeader);

global state;
global i;
global mark;
global run;

run = true;

h = figure(1);
set(h, 'KeyPressFcn', 'keyHit');
hTitle = title(sprintf('Frame 1 of %d', gpsHeader.nframes));
xlim(range(1,:));
ylim(range(2,:));
zlim(range(3,:));
daspect([1 1 1]);
grid on;
view(3);
handles = [];

if(~exist('positionFrames'))
    positionFrames = [];
end
state = 'play';
i = 1;
while(run)
    cGps = RPreadgpsindex(fgps, gpsHeader, i);
    
    x.pos = cGps.pos;
    x.q = RotationMatrixToQuat(cGps.pose);
    handles = drawFrames(1, {x}, 50, [], handles);
    
    if(mark)
        positionFrames = vertcat(positionFrames,i);
        mark = false;
    end
    
    if(strcmpi(state,'play'))
        i = min(i+1,gpsHeader.nframes);
    end
    
    nextIndex = findNextIndexInList(positionFrames,i);
    
    nextFrame = -1;
    if(nextIndex > 0)
        nextFrame = positionFrames(nextIndex);
    else
        nextIndex = length(positionFrames)+1;
    end
        
    
    hTitle.String = sprintf('Frame %d of %d NextPressureIndex = %d, [%f %f %f]', i, gpsHeader.nframes,...
        nextFrame, Pressure(nextIndex,1), Pressure(nextIndex,2), Pressure(nextIndex,3));
    
    pause(1/30);
end

