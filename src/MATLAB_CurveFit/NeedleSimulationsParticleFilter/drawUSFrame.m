function handles = drawUSFrame(measurement, params, handles)
% fc = [0.25 0.25 0.25];
% xd = [measurement.ful measurement.fbl]';
% handles = plot3(xd(:,1),xd(:,2),xd(:,3),'Color',fc,'LineWidth',2);
% xd = [measurement.fbl measurement.fbr]';
% handles = [handles; plot3(xd(:,1),xd(:,2),xd(:,3),'Color',fc,'LineWidth',2)];
% xd = [measurement.fbr measurement.fur]';
% handles = [handles; plot3(xd(:,1),xd(:,2),xd(:,3),'Color',fc,'LineWidth',2)];
% xd = [measurement.fur measurement.ful]';
% handles = [handles;plot3(xd(:,1),xd(:,2),xd(:,3),'Color',fc,'LineWidth',2)];

b1 = measurement.fur-measurement.ful;
b2 = measurement.fbl-measurement.ful;
p = measurement.ful;

if(isempty(handles))
    handles.usFrame = [];
    handles.measurement = [];
end

if(params.drawUSFrame)
    [u v] = ndgrid(0:0.1:1, 0:0.1:1);
    X = p(1)+u*b1(1)+v*b2(1);
    Y = p(2)+u*b1(2)+v*b2(2);
    Z = p(3)+u*b1(3)+v*b2(3);
    if(isempty(handles.usFrame))
        handles.usFrame = surf(X,Y,Z);
    else
        set(handles.usFrame, 'XData', X, 'YData', Y, 'ZData', Z);
    end
    set(handles.usFrame, 'FaceColor', [.25 .25 .25]);
end

if(params.drawMeasurement)
    if(isempty(handles.measurement))
        handles.measurement = scatter3(measurement.pos(1),measurement.pos(2),measurement.pos(3),'filled','MarkerFaceColor',[1 0.4 0]);
    else
        set(handles.measurement, 'XData', measurement.pos(1), 'YData', measurement.pos(2),...
            'ZData', measurement.pos(3));
    end
end
end