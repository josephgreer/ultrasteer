function ShowAccuracyOfFrameIndex(fitPoints, fgps, fim, poly, descriptor, degree, mpp, index, indices)

fpIndex = find(indices == index);

ShowAccuracyOfPointIndex(fitPoints(fpIndex,:), fitPoints, fgps, fim, poly, descriptor, degree, mpp, index, indices);

end