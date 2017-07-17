clear; clc; close all;

v = VideoReader('~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/sPAM Growing Journal/Data/GrowToLight1_Cropped.mp4');
v.CurrentTime = 119;
figure;
positions = [];
while(hasFrame(v))
    im = readFrame(v);
    imshow(im);
    [x y] = ginput(1);
    v.CurrentTime = v.CurrentTime+2;
    positions = vertcat(positions, [x y]);
end
fclose all;

times = linspace(119,v.currentTime,length(positions));
positions = horzcat(times.',positions);

lengths = positions(2:end,:)-positions(1:end-1,:);
lengths = sqrt(sum(lengths.^2,2));
lengths = vertcat(0,lengths);
lengths = cumsum(lengths);
lengths = horzcat(times.',lengths);
lengths(:,2) = lengths(:,2)/lengths(end,2);
lengths(:,2) = lengths(:,2)*55*0.0254;