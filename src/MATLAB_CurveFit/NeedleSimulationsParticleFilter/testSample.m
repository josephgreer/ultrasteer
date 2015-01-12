clear; clc; close all;

npts = 50;
dist = rand(npts,1);
dist = dist/sum(dist);

n = 1e7;
samples = sample(dist, n);

weights = zeros(npts,1);
for i=1:npts
    idxs = find(samples == i);
    weights(i) = length(idxs)/n;
end
