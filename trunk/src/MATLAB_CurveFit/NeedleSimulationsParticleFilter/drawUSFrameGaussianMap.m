clear; clc; close all;

b1 = [1;0;0];
b2 = [0;1;0];
p = [0;0;0];
[u v] = ndgrid(0:0.005:1, 0:0.005:1);
X = p(1)+u*b1(1)+v*b2(1);
Y = p(2)+u*b1(2)+v*b2(2);
Z = p(3)+u*b1(3)+v*b2(3);

%figure('Position', [0 0 1000 1000]);
sobj = surf(X,Y,Z);
load('drawUSFrameColormap');
colormap(cmap);

sigma = diag([0.01; 0.01]);
mu = [0 -0.2];
[u v] = meshgrid(-0.5:0.005:0.5, -0.5:0.005:0.5);

im = reshape(mvnpdf([u(:) v(:)], mu, sigma), size(u));

set(sobj,'CData',im,'FaceColor','texturemap');
im = (im/max(im(:)))*0.4 + 0.4;
set(sobj,'AlphaDataMapping','none','alphadata',im,'facealpha','texture');
figure(1);
set (sobj, 'EdgeColor', 'none');
view(3);
campos([-10; 8; 12]);
camup([0; 1; 0]);
camlookat(sobj);
set(gca, 'visible', 'off');
%axis equal;
export_fig '../../NeedleEstimationJournal/Figures/gaussianFrame.png' -transparent;
   %axis equal;
%im = mvnpdf(

