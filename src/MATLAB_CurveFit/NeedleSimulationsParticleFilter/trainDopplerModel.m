%%%%%%% Used to train p(over needle | doppler response)
clearvars -except data; clc; close all;

if(exist('data') == 0)
    data = [];
end

addpath('../LabelData/');

basePath = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\Joey Greer Research Folder\Data\NeedleScan\8_24_15\Trial3\Insertion\';
dop = fopen(strcat(basePath,'scan.b32'));
dopHeader = ReadHeader(dop);


tol = 0.05;
for i=size(data,1)+1:dopHeader.nframes
    im = RPreadframeindex(dop,dopHeader,i);
    im = im/255;
    imshow(im(:,:,1:3));
    title(sprintf('Frame %d of %d', i, dopHeader.nframes));
    set(gcf, 'Units', 'pixels');
    [x y button] = ginput(2);
    x = round(x); y = round(y);
    im = im(y(1):y(2),x(1):x(2),:);
    r = im(:,:,1);
    g = im(:,:,2);
    b = im(:,:,3);
    bwcoords = find(abs(r-g) < tol & abs(r-b) < tol & ...
        abs(b-g) < tol );
    r(bwcoords) = 0;
    g(bwcoords) = 0;
    b(bwcoords) = 0;
    imbw = 0.2126*r+0.7152*g+0.0722*b;
    
    isDop = 1;
    if(button(1) == 3 || button(2) == 3)
        isDop = 0;
    end
    data = vertcat(data, [sum(imbw(:)) isDop]); 
end

save(strcat(basePath, 'data.mat'), 'data');


nBagging = 5;
offNeedles = find(data(:,2) == 0);
onNeedles = find(data(:,2) == 1);
numObs = length(offNeedles);

Bs = [];
Bsp = [];
for i=1:nBagging
    cOnNeedles = datasample(onNeedles, numObs);
    cOffNeedles = offNeedles;
    
    X = vertcat(data(cOnNeedles,1), data(cOffNeedles,1));
    Y = vertcat(2*ones(numObs, 1), ones(numObs,1));
    B = mnrfit(X,Y);
    Bs = horzcat(Bs,B);
end
B = mean(Bs,2)
figure;
vals = mnrval(B,[-10:.1:100]');
plot([-10:.1:100],vals);

figure;
[Fpos Xipos] = ksdensity(data(onNeedles,1),'function','cdf');%,'support', 'positive');
plot(Xipos, Fpos)
title('p(d | on needle)');
figure;
[Fneg Xineg] = ksdensity(data(offNeedles,1),'function','cdf');
plot(Xineg, 1-Fneg)
title('p(d | off needle)');

figure;
parmhat = lognfit(data(onNeedles,1))
xvals = [0:.1:1000];
plot(xvals, lognpdf(xvals, parmhat(1),parmhat(2)));
