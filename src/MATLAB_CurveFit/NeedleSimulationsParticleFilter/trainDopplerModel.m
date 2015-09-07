%%%%%%% Used to train p(over needle | doppler response)
clearvars -except data; clc; close all;

if(exist('data') == 0)
    data = [];
end

addpath('../LabelData/');

basePath = 'C:\Joey\Data\8_24_15\Trial3\Insertion\';
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
    Y = vertcat(ones(size(cOnNeedles)), 2*ones(size(cOffNeedles)));
    B = mnrfit(X,Y);
    Bs = horzcat(Bs,B);
end
B = mean(Bs,2)
figure;
vals = mnrval(B,[-10:.1:100]');
plot([-10:.1:100],vals);%/sum(vals(:,1)));

%fsolve(@(x)(log((1-x)/x)+B(1)), 0.5)

%data(offNeedles,1) = 1;

figure;
[Fpos Xipos] = ksdensity(data(onNeedles,1),'bandwidth',70);
plot(Xipos, Fpos)
title('p(d | on needle)');
hold on;
[Fneg Xineg] = ksdensity(data(offNeedles,1), 'bandwidth',10);
plot(Xineg, Fneg, 'r');
title('p(d | off needle)');

load('C:/Joey/Data/probDist/testXs.mat', '-ascii');
load('C:/Joey/Data/probDist/testPOverNeedle.mat', '-ascii');
load('C:/Joey/Data/probDist/testPNotOverNeedle.mat', '-ascii');
figure;
plot(testXs, testPOverNeedle);
hold on;
plot(testXs, testPNotOverNeedle,'r');

% 
% bernoulliParam = 0.5;
% 
% Fneg = zeros(size(Fpos));
% %calculate P(dop|not over)/P(dop|over)
% for i=1:length(Xipos)
%     expVal = -B'*[1; Xipos(i)];
%     expVal = expVal-log((1-bernoulliParam)/bernoulliParam);
%     ratio = fsolve(@(x)(expVal-log(x)),0.1);
%     Fneg(i) = ratio*Fpos(i);
%     yep = 0;
% end
% 
% figure;
% plot(Xipos, Fpos);
% hold on;
% Fneg = Fneg/sum(Fneg);
% plot(Xipos, Fneg, 'r');
% % 
% figure;
% parmhat = lognfit(data(onNeedles,1))
% xvals = [0:.1:1000];
% plot(xvals, lognpdf(xvals, parmhat(1),parmhat(2)));
% parmhat = lognfit(data(offNeedles,1))
% hold on;
% xvals = [0:.1:1000];
% plot(xvals, lognpdf(xvals, parmhat(1),parmhat(2)),'r');
