%%%%%%% points = [x1 y1 z1;
%%%%%%%           x2 y2 z2;
%%%%%%%              ...
%%%%%%%           xn yn zn];
%%%%%%  returns corresponding tvals
function tvals = TvalsFromDescriptor(points, descriptor)
offset = transpose(descriptor.offset);
offset = repmat(offset,size(points,1),1);
points = points-offset;
tvals = points*descriptor.dir;
end