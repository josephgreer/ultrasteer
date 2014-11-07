function endPoints = ModelEndPoints(poly, range, descriptor)
degree = size(poly,1);
ts = [];
for i=degree:-1:0
    ts = [ts [range(1)^i; range(2)^i]];
end
ptsX = ts*transpose(poly(1,:));
ptsY = ts*transpose(poly(2,:));
ptsZ = ts*transpose(poly(3,:));

endPoints = [transpose(ptsX); transpose(ptsY); transpose(ptsZ)];
end