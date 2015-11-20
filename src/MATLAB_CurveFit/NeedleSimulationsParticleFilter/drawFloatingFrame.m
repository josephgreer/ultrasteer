function handles = drawFrames(fig,x,scale,params,handles)
if(isempty(handles))
    handles = [];
    thandles = [];
end
figure(fig);

for i=1:length(x)
    hold on;
    R = eye(3);
    if(isfield(x{i},'q'))
        R = QuatToRotationMatrix(x{i}.q)*scale;
    elseif(isfield(x{i},'qdist'))
        R = x{i}.qdist.mu*scale; 
    else
        assert(0);
    end
    x{i}.pos = zeros(3,1);
    xx = x{i}.pos+R(:,1);
    xy = x{i}.pos+R(:,2);
    xz = x{i}.pos+R(:,3);
    
    xd{1} = [x{i}.pos xx]';
    xd{2} = [x{i}.pos xy]';
    xd{3} = [x{i}.pos xz]';
    if(isempty(handles))
        thandles = [thandles;plot3(xd{1}(:,1), xd{1}(:,2), xd{1}(:,3),'r', 'LineWidth', 2)];
        thandles = [thandles;plot3(xd{2}(:,1), xd{2}(:,2), xd{2}(:,3),'g', 'LineWidth', 2)];
        thandles = [thandles;plot3(xd{3}(:,1), xd{3}(:,2), xd{3}(:,3),'b', 'LineWidth', 2)];
    else
        set(handles(3*(i-1)+1), 'XData', xd{1}(:,1),  'YData',  xd{1}(:,2), 'ZData', xd{1}(:,3));
        set(handles(3*(i-1)+2), 'XData', xd{2}(:,1),  'YData',  xd{2}(:,2), 'ZData', xd{2}(:,3));
        set(handles(3*(i-1)+3), 'XData', xd{3}(:,1),  'YData',  xd{3}(:,2), 'ZData', xd{3}(:,3));
    end
end

xlim([-20 20]);
ylim([-20 20]);
zlim([-20 20]);
daspect([1 1 1]);

if(isempty(handles))
    handles = thandles;
end
end