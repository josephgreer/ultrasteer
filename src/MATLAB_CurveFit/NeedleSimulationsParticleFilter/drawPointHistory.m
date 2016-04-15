% draws a point history on figure fnum
function h = drawPointHistory(xhist, fnum, color, handle)
figure(fnum);
hold on;
pos = cell2mat(xhist);
pos = [pos.pos]';
if(isempty(handle))
    h = scatter3(pos(:,1), pos(:,2), pos(:,3), 'filled','MarkerFaceColor',color);
else
    set(handle, 'XData', pos(:,1),  'YData',  pos(:,2), 'ZData', pos(:,3));
    set(handle, 'MarkerFaceColor', color);
    h = handle;
end
end