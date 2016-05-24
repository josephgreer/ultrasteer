function h = draw2DCircle(center, rad, fillColor, edgeColor, handle)
if(isempty(handle))
    handle = rectangle('Curvature', [1 1]);
end
handle.Position = [center(1)-rad center(2)-rad 2*rad 2*rad];
handle.Curvature = [1 1];
handle.FaceColor = fillColor;
handle.EdgeColor = edgeColor;

h = handle;
end