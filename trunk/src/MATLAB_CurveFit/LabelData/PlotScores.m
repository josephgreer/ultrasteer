function PlotScores(fgps, fitPoints, indices, poly, descriptor, degree, mpp)

tvals = TvalsFromDescriptor(fitPoints, descriptor);
range = [min(tvals); max(tvals)];
header = ReadHeader(fgps);


norms = [];
for i=1:size(fitPoints,1)
    gps = RPreadgpsindex(fgps, header, indices(i));
    intersection = IntersectFrameAndPoly(header, gps, mpp, poly, degree, range, transpose(fitPoints(i,[1:3])));
    yep = norm(intersection-transpose(fitPoints(i,[1:3])));
    norms = [norms; yep];
end

figure(12);
plot([1:size(norms,1)], norms);


travels = [];
for i=1:size(indices,1)
    index = indices(i);
    
    travel = 0;
    if(index - 1 > 0 && index + 1 < size(indices,1))
        gps1 = RPreadgpsindex(fgps, header, index-1);
        gps2 = RPreadgpsindex(fgps, header, index);
        travel = max(travel,FrameTravel(header, mpp, gps1,gps2));
    elseif(index - 1 > 0)
        gps1 = RPreadgpsindex(fgps, header, index-1);
        gps2 = RPreadgpsindex(fgps, header, index);
        travel = max(travel,FrameTravel(header, mpp, gps1,gps2));
    else
        gps1 = RPreadgpsindex(fgps, header, index);
        gps2 = RPreadgpsindex(fgps, header, index+1);
        travel = max(travel,FrameTravel(header, mpp, gps1,gps2));
    end
    travels = [travels; travel];
end
figure(14);
plot([1:size(indices,1)], travels, 'r');
[r p] = corrcoef([norms travels])

end