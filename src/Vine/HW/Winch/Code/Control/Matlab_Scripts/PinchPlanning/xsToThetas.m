function thetas = xsToThetas(xs)
deltas = xs(2:end,:)-xs(1:end-1,:);
ls = sqrt(sum(deltas.^2,2));
thetas = atan2(deltas(:,2),deltas(:,1));
thetas = thetas(2:end)-thetas(1:end-1);
thetas = [[0; cumsum(ls(2:end))] [thetas; 0]];
end