function drawWorkspace(thetas, strengths)
npoints = 100;

thetauvs = zeros(2, length(thetas));
for j=1:length(thetas)
    thetauvs(:,j) = [cos(thetas(j)); sin(thetas(j))];
end

ts = linspace(0,2*pi,npoints);
xs = cos(ts);
ys = sin(ts);

for i=1:length(ts)
    mag = 0;
    for j=1:length(thetas)
        mag = mag+max(strengths(j)*dot(thetauvs(:,j), [xs(i); ys(i)]), 0);
    end
    xs(i) = xs(i)*mag;
    ys(i) = ys(i)*mag;
end

plot(xs, ys, 'k');
end