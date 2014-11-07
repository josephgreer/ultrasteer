function [index score] = WorstFits(fgps, fim, fitPoints, points, indices, poly, descriptor, degree, mpp)
nworst = str2double(inputdlg('n worst please'));

tvals = TvalsFromDescriptor(fitPoints, descriptor);
range = [min(tvals), max(tvals)];
imheader = ReadHeader(fim);
header = ReadHeader(fgps);


norms = [];
for i=1:size(fitPoints,1)
    gps = RPreadgpsindex(fgps, header, indices(i));
    intersection = IntersectFrameAndPoly(header, gps, mpp, poly, degree, range, transpose(fitPoints(i,:)));
    yep = norm(intersection-transpose(fitPoints(i,[1:3])));
    norms = [norms; yep];
end

arr = [norms indices transpose([1:size(norms,1)])];

[s si] = sort(arr(:,1), 'descend');
arrSorted = arr(si,:);

for i=1:nworst
    fpIndex = find(arrSorted(i, 2) == indices);
    figure(1)
    h = scatter3(fitPoints(fpIndex,1), fitPoints(fpIndex,2), fitPoints(fpIndex,3), 200, 'r');
    ShowAccuracyOfFrameIndex(fitPoints, fgps, fim, poly, descriptor, degree, mpp, arrSorted(i,2), indices);
    delete(h);
end



end