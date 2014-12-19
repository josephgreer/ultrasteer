function handles = drawFrames(fig,x,scale)
handles = [];
figure(fig);

for i=1:length(x)
    hold on;
    R = QuatToRotationMatrix(x{i}.q)*scale;
    xx = x{i}.pos+R(:,1);
    xy = x{i}.pos+R(:,2);
    xz = x{i}.pos+R(:,3);
    
    xd = [x{i}.pos xx]';
    handles = [handles;plot3(xd(:,1), xd(:,2), xd(:,3),'r', 'LineWidth', 2)];
    hold on;
    xd = [x{i}.pos xy]';
    handles = [handles;plot3(xd(:,1), xd(:,2), xd(:,3),'g', 'LineWidth', 2)];
    hold on;
    xd = [x{i}.pos xz]';
    handles = [handles;plot3(xd(:,1), xd(:,2), xd(:,3),'b', 'LineWidth', 2)];
    hold on;
end
end