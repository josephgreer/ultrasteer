function RespKeyPress(obj, event)
handles = guidata(obj);

if event.Character == 'q'
    handles.quit = 1;
elseif event.Character == 's'
    handles.index = max(handles.index-1,1);
    drawframe(handles);
elseif event.Character == 'd'
    handles.index = min(handles.index+1,handles.info.NumberOfFrames);
    drawframe(handles);
elseif event.Character == 'l'
    pt = ginput(1);
    handles.ptMap(num2str(handles.index)) = pt;
    drawframe(handles);
end

guidata(obj,handles);
end

function drawframe(handles)
    data = dicomread(handles.inputFile,'frames',handles.index);
    imagesc(data(:,:),handles.scale);
    axis equal;
    
    if isKey(handles.ptMap, num2str(handles.index))
        hold on;
        pt = handles.ptMap(num2str(handles.index));
        scatter(pt(1),pt(2));
        hold off;
    end
end