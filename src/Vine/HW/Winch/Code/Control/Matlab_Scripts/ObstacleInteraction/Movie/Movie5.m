clear; clc; close all;
     
path = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/Maze3/Trial5.jpg';
im = imread(path);  

vid = VideoWriter('Movie5.avi');
vid.FrameRate = 10;
open(vid);

imshow(im);
tightfig;
hold on;

basePoint = [406 495];
startPoint = [452 299];
scatter(basePoint(1),basePoint(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);
% scatter(pts(:,1), pts(:,2));

v = 5;

h = plot([basePoint(1); startPoint(1)], [basePoint(2); startPoint(2)],'k','LineWidth',5);
ht = scatter(startPoint(1),startPoint(2),'r','LineWidth',3);

endPoint = [452.1 299.1];
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

basePoint2 = startPoint;
plot([basePoint(1); basePoint2(1)], [basePoint(2); basePoint2(2)],'k','LineWidth',5);
scatter(basePoint2(1),basePoint2(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);

startPoint = endPoint;
endPoint = [536  283];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint2; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end

endPoint = startPoint+a*delta;
h2 = scatter(endPoint(1), endPoint(2), 'b', 'LineWidth', 3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);
delete(h2);

startPoint = [618  273];
endPoint = [620  229];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint2; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end

startPoint = [694 219];
endPoint = [694.1  215.1];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint2; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end

endPoint = startPoint+a*delta;
basePoint3 = [656 231];
plot([basePoint2(1); basePoint3(1)], [basePoint2(2); basePoint3(2)],'k','LineWidth',5);
scatter(basePoint3(1),basePoint3(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);

endPoint = [716  241];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint3; cPoint];
    set(h, 'XData', cpts(:,1), 'YData', cpts(:,2));
    set(ht,'XData', cPoint(1),'YData', cPoint(2));
    currFrame = getframe;
    writeVideo(vid, currFrame);
    pause(0.1);
end

endPoint = startPoint+a*delta;
h2 = scatter(endPoint(1), endPoint(2), 'b', 'LineWidth', 3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);
delete(h2);

startPoint = endPoint;
endPoint = [726  533];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint3; cPoint];
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