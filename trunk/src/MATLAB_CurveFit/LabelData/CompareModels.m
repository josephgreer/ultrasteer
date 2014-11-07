function rmsError = CompareModels(poly1, descriptor1, range1, poly2, descriptor2)
pPoints1 = PolyPoints(range1, poly1, 1000);
tvals = TvalsFromDescriptor(pPoints1,descriptor2);
degree = size(poly2,1);
Ain = [];
for i=degree:-1:0
    Ain = [Ain tvals.^i];
end
pPoints2 = [];
pPoints2 = [pPoints2 Ain*transpose(poly2(1,:))];
pPoints2 = [pPoints2 Ain*transpose(poly2(2,:))];
pPoints2 = [pPoints2 Ain*transpose(poly2(3,:))];

delta = pPoints1-pPoints2;
error = transpose(delta)*delta;
rmsError = sqrt(trace(error))/size(pPoints1,1);
end