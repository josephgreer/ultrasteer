% density corresponds to number of obstacles in each direction
function walls = GenerateForestMap(density)
d = 500;

xs = linspace(-d,d,density);

[xs,ys] = meshgrid(xs,xs);

xs = xs(:);
ys = ys(:);

len = d/10;

thetas = unifrnd(0,2*pi,length(xs),1);

walls = [];
for i=1:length(xs)
    p0 = [xs(i); ys(i)];
    
    dir = [cos(thetas(i)); sin(thetas(i))];
    
    e1 = p0-len/2*dir;
    e2 = p0+len/2*dir;
    
    walls = vertcat(walls,[e1.' e2.']);
end

end