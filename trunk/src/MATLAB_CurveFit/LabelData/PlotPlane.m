function sobj = PlotPlane(p,b1, b2)
[u v] = ndgrid(0:.1:1, 0:.1:1);
X = p(1)+u*b1(1)+v*b2(1);
Y = p(2)+u*b1(2)+v*b2(2);
Z = p(3)+u*b1(3)+v*b2(3);
sobj = surf(X,Y,Z);
%set(sobj, 'facecolor', 'none', 'edgecolor', 'none');
end