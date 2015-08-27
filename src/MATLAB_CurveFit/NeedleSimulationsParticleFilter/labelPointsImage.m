clear; clc; close all;

pathim = 'C:\Joey\Data\8_24_15\Trial7\photo.JPG';
pathpoints = 'C:\Joey\Data\8_24_15\Trial7\tipHistoryUS.mat';

im = imread(pathim);
im = permute(im, [2 1 3]);
for i=1:3
    im(:,:,i) = fliplr(im(:,:,i));
    im(:,:,i) = flipud(im(:,:,i));
end
imshow(im);

load(pathpoints, '-ascii')
tipHistoryUS = tipHistoryUS-repmat(mean(tipHistoryUS,1),size(tipHistoryUS,1),1);
tipHistoryUS = tipHistoryUS';

[U S V] = svd(tipHistoryUS);

projection = U(:,[1 2])';
projectedTipHistoryUS = projection*tipHistoryUS;
projectedTipHistoryUS = projectedTipHistoryUS';
projectedTipHistoryUS = projectedTipHistoryUS-repmat(projectedTipHistoryUS(1,:), size(projectedTipHistoryUS,1), 1);

point1 = projectedTipHistoryUS(1,:);
point2 = projectedTipHistoryUS(5,:);
delta = point2-point1;

theta = angle(delta(1)+delta(2)*j);
theta = -theta+pi;
projectedTipHistoryUS = projectedTipHistoryUS';
projectedTipHistoryUS = projectedTipHistoryUS-repmat(projectedTipHistoryUS(:,1),1,size(projectedTipHistoryUS,2));
projectedTipHistoryUS = [cos(theta) -sin(theta); sin(theta) cos(theta)]*projectedTipHistoryUS;
projectedTipHistoryUS = projectedTipHistoryUS';
projectedTipHistoryUS = projectedTipHistoryUS/projectedTipHistoryUS(end,2);


figure;
scatter(projectedTipHistoryUS(:,1), projectedTipHistoryUS(:,2));
%axis equal;
daspect([1 1 1]);

figure(1);

photoPoints = [];
while(1)
    [x y button] = ginput(1);
    if(button == 3)
        break;
    end
    photoPoints = vertcat(photoPoints, [x y]);
end
figure;
photoPoints = photoPoints-repmat(photoPoints(1,:),size(photoPoints,1),1);
photoPoints = photoPoints/photoPoints(end,2);
scatter(photoPoints(:,1), photoPoints(:,2));
%axis equal;
daspect([1 1 1]);