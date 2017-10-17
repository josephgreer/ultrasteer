clear; clc; close all;
     
path = '~/Dropbox (Stanford CHARM Lab)/Joey Greer Research Folder/Papers/AnalysisOfGrowingRobotActuation/Data/Maze3/Trial4.jpg';
im = imread(path); 

vid = VideoWriter('Movie4.avi');
vid.FrameRate = 10;
open(vid);    

imshow(im);
tightfig;
hold on;

basePoint = [392 497];
startPoint = [390 383];
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

endPoint = [386 383];
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

basePoint2 = [390 397];
plot([basePoint(1); basePoint2(1)], [basePoint(2); basePoint2(2)],'k','LineWidth',5);
scatter(basePoint2(1),basePoint2(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);

endPoint = startPoint + a*delta;
startPoint = endPoint;
endPoint = [326  357];
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

startPoint = [192  265];
endPoint = [164  267];
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
basePoint3 = endPoint;
plot([basePoint2(1); basePoint3(1)], [basePoint2(2); basePoint3(2)],'k','LineWidth',5);
scatter(basePoint3(1),basePoint3(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);

startPoint = endPoint;
endPoint = [86  307];
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

startPoint = [40  331];
endPoint = [34  479];
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
basePoint4 = [98 373];
plot([basePoint3(1); basePoint4(1)], [basePoint3(2); basePoint4(2)],'k','LineWidth',5);
scatter(basePoint4(1),basePoint4(2),'b','LineWidth',3);
for i=1:10
    currFrame = getframe;
    writeVideo(vid, currFrame);
end
pause(1);

startPoint = endPoint;
endPoint = [36  535];
delta = endPoint-startPoint;
mag = norm(delta);
delta = delta/mag;
for a=0:v:mag
    cPoint = startPoint+delta*a;
    
    cpts = [basePoint4; cPoint];
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