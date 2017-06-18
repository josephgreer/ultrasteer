function handles = drawVine(xs,d,cameraParams, handles)
[pos,size,handles] = StiffnessesToImagePoint(xs,d,cameraParams,handles);
if(~isfield(handles,'imTarget'))
    handles.imTarget = scatter(pos(1),pos(2),size*10,'r','filled');
else
    set(handles.imTarget,'XData',pos(1),'YData',pos(2));
end
ylim([-240 240]);
xlim([-320 320]);
daspect([1 1 1]);
box on;
grid on;
subplot(1,2,1);

end