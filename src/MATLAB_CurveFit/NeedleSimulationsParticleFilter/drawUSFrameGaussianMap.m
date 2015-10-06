clear; clc; close all;

flat = 1;

b1 = [1;0;0];
b2 = [0;1;0];
p = [0;0;0];
aspect = 500/600;
npix = 600;
linespacing = 100;
[v u] = ndgrid(0:1/npix:1, 0:1/npix:aspect);
X = p(1)+u*b1(1)+v*b2(1);
Y = p(2)+u*b1(2)+v*b2(2);
Z = p(3)+u*b1(3)+v*b2(3);

%figure('Position', [0 0 1000 1000]);
sobj = surf(X,Y,Z);
if(flat)
    load('drawUSFrameColormapFlat');
else
    load('drawUSFrameColormap');
end
colormap(cmap);
caxis([0 1]);

cmap = 0.25*ones(size(cmap));
colormap(cmap);
caxis([0 1]);

sigma = diag([0.01; 0.01]);
mu = [-0.1 -0.2];
[u v] = meshgrid([-aspect/2:1/npix:aspect/2],[-1/2:1/npix:1/2]);

im = 0.25*ones(size(u));%reshape(mvnpdf([u(:) v(:)], mu, sigma), size(u))+0.5;
im = im/max(im(:));

c = [1:linespacing:size(im,2)];
r = [1:linespacing:size(im,1)];

if(flat)
    ima = 0.25*ones(size(im));%(im/max(im(:)))*0.6+0.2;
    %ima = (im/max(im(:)))*0.0+1;
else
    ima = (im/max(im(:)))*0.6+0.2;
end
for i=1:length(c)
    ima(:,c(i)) = 1;
    im(:,c(i)) = 0;
end
for i=1:length(r)
    ima(r(i),:) = 1;
    im(r(i),:) = 0;
end

set(sobj,'CData',im,'FaceColor','texturemap');
set(sobj,'AlphaDataMapping','none','alphadata',ima,'facealpha','texture');
set (sobj, 'EdgeColor', 'none');
if(flat > 0)
    view(2);
else
    view(3);
    campos([-10; 8; 6]);
    camup([0; 1; 0]);
    camlookat(sobj);
end
axis equal;
set(gca, 'visible', 'off');
%export_fig '../../NeedleEstimationJournal/Figures/gaussianFrame.png' -transparent;

