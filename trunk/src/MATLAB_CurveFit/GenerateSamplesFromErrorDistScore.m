function samples = GenerateSamplesFromErrorDistScore(distScore, zrange, polyX, polyY, nmeasurement)
offsets = randn(nmeasurement, 3);
offsets = normr(offsets);

zcoords = unifrnd(zrange(1), zrange(2), nmeasurement, 1);
zcoords = [zcoords.^3 zcoords.^2 zcoords.^1 zcoords.^0];
coords = [zcoords(:,3) zcoords*polyX zcoords*polyY];
samples = coords+[distScore(:,1) distScore(:, 1) distScore(:, 1)].*offsets;
end