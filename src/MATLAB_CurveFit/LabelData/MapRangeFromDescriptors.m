function rangeOut = MapRangeFromDescriptors(descriptor1, poly1, range1, descriptor2)
degree = size(poly1,1);
ts = [];
for i=degree:-1:0
    ts = [ts [range1(1)^i; range1(2)^i]];
end
ptsX = ts*transpose(poly1(1,:));
ptsY = ts*transpose(poly1(2,:));
ptsZ = ts*transpose(poly1(3,:));

tvals = TvalsFromDescriptor([ptsX ptsY ptsZ], descriptor2);
rangeOut = [min(tvals) max(tvals)];
end