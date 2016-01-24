%%%%%%% Used to train p(over needle | doppler response)
clearvars -except data; clc; close all;
if(exist('data') == 0)
    data = [];
end

addpath('../LabelData/');

basePath = 'C:\Joey\Data\12_16_15\1mm\Trial1\';
if(exist(strcat(basePath,'data.mat'), 'file'))
    data = [];
    load(strcat(basePath,'data.mat'));
end
dop = fopen(strcat(basePath,'scan.b32'));
dopHeader = ReadHeader(dop);


tol = 0.05;
for i=size(data,1)+1:dopHeader.nframes
    im = RPreadframeindex(dop,dopHeader,i);
    im = im/255;
    %imshow(im(:,:,1:3));
    display(sprintf('Frame %d of %d\n', i, dopHeader.nframes));
    %set(gcf, 'Units', 'pixels');
%     [x y button] = ginput(1);
%     x = round(x); y = round(y);
    %im = im(y(1):y(2),x(1):x(2),:);
    r = im(:,:,1);
    g = im(:,:,2);
    b = im(:,:,3);
    bwcoords = find(abs(r-g) < tol & abs(r-b) < tol & ...
        abs(b-g) < tol );
    r(bwcoords) = 0;
    g(bwcoords) = 0;
    b(bwcoords) = 0;
    imbw = 0.2126*r+0.7152*g+0.0722*b;
    
    isDop = 0;
%     isDop = 1;
%     if(button(1) == 3)% || button(2) == 3)
%         isDop = 0;
%     end
    data = vertcat(data, [min(sum(imbw(:)),1e10) isDop]); 
end


pause;

onOffPath = strcat(basePath, 'OnOff.txt');
if(exist(onOffPath)) 
    onOff = load(onOffPath, '-ascii');
    on = 1;
    for jj=1:length(onOff)-1
        data(onOff(jj):onOff(jj+1)-1,end) = on;
        on = 1-on;
    end
end

save(strcat(basePath, 'data.mat'), 'data');

nPad = 50;
data = vertcat(data, [zeros(nPad,1), ones(nPad,1)]);

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
vals = mnrval(B,[-10:.1:1e4]');
plot([-10:.1:1e4],vals);%/sum(vals(:,1)));

Bs = [];
Bsp = [];
for i=1:nBagging
    cOnNeedles = datasample(onNeedles, numObs);
    cOffNeedles = offNeedles;
    
    X = vertcat(data(cOnNeedles,1), data(cOffNeedles,1));
    Y = vertcat(ones(size(cOnNeedles)), zeros(size(cOffNeedles)));
    ws = vertcat(1.5*ones(size(cOnNeedles)),ones(size(cOffNeedles)));
    B = glmfit(X,Y,'binomial','weights',ws);
    Bs = horzcat(Bs,B);
end
Boutli = mean(Bs,2)
figure;
vals = glmval(B,[-10:.1:1e4]','logit');
plot([-10:.1:1e4],vals);%/sum(vals(:,1)));

%fsolve(@(x)(log((1-x)/x)+B(1)), 0.5)

%data(offNeedles,1) = 1;
figure;
[Fpos Xipos] = ksdensity(data(onNeedles,1),'bandwidth',80);
plot(Xipos, Fpos)
title('p(d | on needle)');
res = [Xipos; Fpos];
save(strcat(basePath, 'pdopoverneedle.dat'), 'res', '-ascii');
hold on;
[Fneg Xineg] = ksdensity(data(offNeedles,1), 'bandwidth',100);
plot(Xineg, Fneg, 'r');
title('p(d | off needle)');
res = [Xineg; Fneg];
save(strcat(basePath, 'pdopnotoverneedle.dat'), 'res', '-ascii');
% load('C:/Joey/Data/probDist/testXs.mat', '-ascii');
% load('C:/Joey/Data/probDist/testPOverNeedle.mat', '-ascii');
% load('C:/Joey/Data/probDist/testPNotOverNeedle.mat', '-ascii');
% figure;
% plot(testXs, testPOverNeedle);
% hold on;
% plot(testXs, testPNotOverNeedle,'r');

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
