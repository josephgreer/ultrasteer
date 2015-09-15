function P = lutDistributionP(x, dist, params)
xx = dist(1,:);
p = dist(2,:);

dist = abs(x-xx);
[minVal minIdx] = min(dist);

P = p(minIdx);
end