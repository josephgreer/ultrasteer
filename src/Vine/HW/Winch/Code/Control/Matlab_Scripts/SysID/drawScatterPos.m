function handles = drawScatterPos(pos,handles)
if(isempty(handles))
    handles = scatter3(pos(:,1),pos(:,2),pos(:,3));
else
    set(handles,'XData',pos(:,1),'YData',pos(:,2),'ZData',pos(:,3));
end
end