function output_txt = DataCursorIndex(obj,event_obj, indices, fitPoints)
% Display the position of the data cursor
% obj          Currently not used (empty)
% event_obj    Handle to event object
% output_txt   Data cursor text string (string or cell array of strings).

pos = get(event_obj,'Position');

% Import x and y
x = get(get(event_obj,'Target'),'XData');
y = get(get(event_obj,'Target'),'YData');
z = get(get(event_obj,'Target'),'ZData');

% Find index
index_x = find(abs(fitPoints(:,1)-pos(1))<.1);
index_y = find(abs(fitPoints(:,2)-pos(2))<.1);
index_z = find(abs(fitPoints(:,3)-pos(3))<.1);
index = intersect(index_x,intersect(index_y,index_z));
oindex = indices(index);

% Set output text
output_txt = {['X: ',num2str(pos(1),4)], ...
              ['Y: ',num2str(pos(2),4)], ...
              ['Z: ',num2str(pos(3),4)], ...
              ['Index: ', num2str(index)], ...
              ['OIndex: ', num2str(oindex)]};
end