function ScaleDataPlot(scale)
h = gcf; %current figure handle

axesObjs = get(h, 'Children');  %axes handles
dataObjs = get(axesObjs, 'Children');

objTypes = get(dataObjs, 'Type');

xdata = get(dataObjs, 'XData');  %data from low-level grahics objects
ydata = get(dataObjs, 'YData');

scalar = @(x)(x*scale);

xdata = cellfun(scalar, xdata, 'UniformOutput', false);
ydata = cellfun(scalar, ydata, 'UniformOutput', false);

for i=1:length(dataObjs)
    set(dataObjs(i),'XData',xdata{i});
    set(dataObjs(i),'YData',ydata{i});
end

% set(dataObjs,'XData',xdata);
% set(dataObjs,'YData',ydata);
end