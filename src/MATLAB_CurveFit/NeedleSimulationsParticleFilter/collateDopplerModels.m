clear; clc; close all;

paths = {'C:\Joey\Data\1_7_16\05mm\Trial1\data.mat', 'C:\Joey\Data\1_7_16\05mm\Trial2\data.mat',...
    'C:\Joey\Data\1_7_16\05mm\Trial3\data.mat', 'C:\Joey\Data\1_7_16\05mm\Trial4\data.mat',...
    'C:\Joey\Data\1_7_16\05mm\Trial5\data.mat'}
%paths = {'C:\Joey\Data\1_7_16\05mm\Trial1\data.mat'};

basePath = 'C:\Joey\Data\1_7_16\05mm\Trial1\';

data = load(paths{1});
data = data.data;
for i=2:length(paths)
    currData = load(paths{i});
    data = vertcat(data, currData.data);
end

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
    ws = vertcat(1.1*ones(size(cOnNeedles)),ones(size(cOffNeedles)));
    B = glmfit(X,Y,'binomial','weights',ws);
    Bs = horzcat(Bs,B);
end
Boutli = mean(Bs,2)
figure;
vals = glmval(B,[-10:.1:1e4]','logit');
plot([-10:.1:1e4],vals);%/sum(vals(:,1)));

%fsolve(@(x)(log((1-x)/x)+B(1)), 0.5)

%data(offNeedles,1) = 1;


nPad = 0;
nPad = nPad*length(paths);
mults = randn(nPad,1)*0.5 + 1;
base = 50;
data = vertcat(data, [base*mults, ones(nPad,1)]);
offNeedles = find(data(:,2) == 0);
onNeedles = find(data(:,2) == 1);

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