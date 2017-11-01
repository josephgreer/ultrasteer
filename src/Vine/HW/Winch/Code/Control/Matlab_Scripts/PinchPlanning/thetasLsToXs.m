function xs = thetasLsToXs(thetas, ls, p0)
runThetas = cumsum(thetas);
rls = cumsum(ls);
deltas = [cos(runThetas) sin(runThetas)].*repmat(ls(1:end),1,2);
xs = vertcat(p0.', deltas);
xs = cumsum(xs, 1);
end