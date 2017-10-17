clear; clc; close all;

pts = [374.0000  514.0000;...
         166.0000  345.0000;...
         170.0000  337.0000;...
         188.0000  298.0000;...
         186.0000  260.0000;...
         228.0000  236.0000];
     
path = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/Maze3/Trial1.jpg';
im = imread(path);  

vid = VideoWriter('Movie1.avi');
vid.FrameRate = 10;
open(vid);       

imshow(im);
tightfig;
pause(1);
hold on;
scatter(pts(1,1),pts(1,2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);
% scatter(pts(:,1), pts(:,2));

v = 5;

currPoints = [1;2]
h = plot(pts(currPoints,1), pts(currPoints,2),'k','LineWidth',5);
ht = scatter(pts(2,1),pts(2,2),'r','LineWidth',3);

delta = pts(3,:)-pts(2,:);
mag = norm(delta);
delta = delta/mag;

basePoint = pts(1,:);
startPoint = pts(2,:)
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end

endPoint = startPoint+a*delta;
plot([pts(1,1) endPoint(1)], [pts(1,2) endPoint(2)],'k','LineWidth',5);
scatter(endPoint(1),endPoint(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);

basePoint = endPoint;
startPoint = [174.0000  316.0000];
endPoint = [184.0000  298.0000];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end

h2 = scatter(184, 298, 'b', 'LineWidth', 3);
pause(1);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
delete(h2);

t2 = plot([basePoint(1); endPoint(1)], [basePoint(2);endPoint(2)],'k','LineWidth',5);
delete(t2);

basePoint = pts(3,:);
startPoint = [209  253];
endPoint = [229.0000  237.0000];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end

h2 = scatter(endPoint(1), endPoint(2), 'b', 'LineWidth', 3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);

delete(h2);

basePoint = pts(3,:);
startPoint = [262  187];
endPoint = [292  191];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end


basePoint2 = [190 311];
plot([basePoint(1); basePoint2(1)], [basePoint(2); basePoint2(2)],'k','LineWidth',5);
scatter(basePoint2(1),basePoint2(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);
basePoint = basePoint2;

endPoint = startPoint+a*delta;
h2 = scatter(endPoint(1), endPoint(2), 'b', 'LineWidth', 3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);
delete(h2);

startPoint = [428  37];
endPoint = [550  35];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end


basePoint = [446,115];
scatter(basePoint(1),basePoint(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);


plot([basePoint2(1);basePoint(1)],[basePoint2(2);basePoint(2)],'k','LineWidth',5);
scatter(basePoint(1),basePoint(2),'b','LineWidth',3);
startPoint = [550 35];
endPoint = [684  31];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end

endPoint = startPoint+a*delta;
h2 = scatter(endPoint(1),endPoint(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);
delete(h2);

startPoint = [718 13];
endPoint = [718.1  13.1];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:0
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end


for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
close(vid);