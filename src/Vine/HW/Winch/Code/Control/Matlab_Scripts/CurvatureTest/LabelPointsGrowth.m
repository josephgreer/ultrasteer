clear; clc; close all;
directory = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/CurvatureTest/'; 

file = '0_05.mov';
startTime = 108;
endTime = 183;

% file = '0_075.mov';
% startTime = 60;
% endTime = 120;

% file = '0_1.mov';
% startTime = 32;
% endTime = 82;

% file = '0_125.mov';
% startTime = 47;
% endTime = 114;

v = VideoReader(strcat(directory,file));
v.CurrentTime = startTime;

points = [];
first = true;
figure;
while hasFrame(v)
    video = readFrame(v);
    imshow(video);
    if(first)
        [x y] = ginput(5);
    else
        [x y] = ginput(1);
    end
    first = false;
    points = vertcat(points, [x y]);
    v.CurrentTime = v.CurrentTime+3;
    if(v.CurrentTime > endTime)
        break;
    end
end

display('Mark the line');
[x y] = ginput(1);
points = vertcat(points, [x y]);
[x y] = ginput(1);
points = vertcat(points, [x y]);