% draws a point history on figure fnum
function drawPointHistory(xhist, fnum, color)
figure(fnum);
hold on;
pos = cell2mat(xhist);
pos = [pos.pos]';
scatter3(pos(:,1), pos(:,2), pos(:,3), 'filled','MarkerFaceColor',color);
end