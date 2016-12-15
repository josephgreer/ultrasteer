function h = drawRobot(handles, centerPos, thetas, qs)
robotRad = 30;
if(~isfield(handles,'robotHandle'))
    handles.robotHandle = [];
end
handles.robotHandle = draw2DCircle(centerPos, robotRad, 'none', [0 0 0], handles.robotHandle);
if(~isfield(handles,'actuatorHandles'))
    handles.actuatorHandles = cell(size(qs));
end
if(~isfield(handles,'textHandles'))
    handles.textHandles = cell(size(qs));
    for i=1:length(qs)
        textCenter = centerPos+[cos(thetas(i)); sin(thetas(i))]*(robotRad+30);
        handles.textHandles{i} = text(textCenter(1), textCenter(2), sprintf('%d, %.2f', i, qs(i)));
    end
end
for i=1:length(qs)
    actuatorRad = 5;
    actuatorCenter = centerPos+[cos(thetas(i)); sin(thetas(i))]*(robotRad-actuatorRad);
    textCenter = centerPos+[cos(thetas(i)); sin(thetas(i))]*(robotRad+30);
    handles.actuatorHandles{i} = draw2DCircle(actuatorCenter, actuatorRad, [1 1-qs(i) 1-qs(i)], [0 0 0], handles.actuatorHandles{i});
    handles.textHandles{i}.String = sprintf('%d, %.2f', i, qs(i)); 
    handles.textHandles{i}.Position = textCenter;
end

h = handles;
daspect([1 1 1]);
end