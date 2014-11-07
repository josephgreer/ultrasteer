function coordsScore = GenerateOutliers(scoreMean, scoreVariance, distMax, nmeasurement,zrange, polyX, polyY)
    scores = mvnrnd(scoreMean, scoreVariance, nmeasurement);
    
    
    zcoords = unifrnd(zrange(1), zrange(2), nmeasurement, 1);
    zcoords = [zcoords.^3 zcoords.^2 zcoords.^1 zcoords.^0];
    coords = [zcoords(:,3) zcoords*polyX zcoords*polyY];
    
    offsets = unifrnd(-distMax/sqrt(2), distMax/sqrt(2), [nmeasurement 2]);
    coords = coords+[zeros(size(coords,1),1) offsets];
    
    coordsScore = [coords scores];
end