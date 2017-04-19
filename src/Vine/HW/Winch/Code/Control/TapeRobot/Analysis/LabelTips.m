clear; clc; close all;

v = VideoReader('~/Dropbox (Stanford CHARM Lab)/Science Submission/GrowToLight_Cropped_SpedUp.mp4');
v.CurrentTime = 0;

figure;
points = [];
while(hasFrame(v))
    frame = readFrame(v);
    imshow(frame);
    [x y] = ginput(1);
    
    points = vertcat(points, [v.CurrentTime x y]);
    v.CurrentTime = v.CurrentTime+0.25;
end


fclose all;