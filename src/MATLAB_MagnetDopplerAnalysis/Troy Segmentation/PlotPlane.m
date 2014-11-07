function sobj = PlotPlane(p,b1, b2,im,alpha)
[u v] = ndgrid(0:1, 0:1);
X = p(1)+u*b1(1)+v*b2(1);
Z = p(2)+u*b1(2)+v*b2(2);
Y = p(3)+u*b1(3)+v*b2(3);
h = surface(X,Y,Z);

im = permute(im, [2 1 3]);
alpha = permute(alpha, [2 1]);

set(h,'CData',im,'FaceColor','texturemap')
set(h, 'EdgeColor', 'None');
set(h,'AlphaDataMapping','direct','alphadata',alpha,'facealpha','texture');
%set(sobj, 'facecolor', 'none', 'edgecolor', 'none');
end
