clear all;
close all;


% plot 1
npoints = 20;
distMax = 10;
distMaxOutlier = 75;
outlierFrac = 1;
curvePoints = [51 -10 311; 51 0 241; 80 15 107; 132 20 45];  %oracle polynomial points

% use z as a descriptor variable and regress x and y off z
zrange = [min(curvePoints(:,3)) max(curvePoints(:, 3))];
Zplot = transpose([min(curvePoints(:, 3)):.5:max(curvePoints(:,3))]);
Zcoeffs = [Zplot.^3 Zplot.^2 Zplot.^1 Zplot.^0];

%x as a function of z
Ain = [curvePoints(:, 3).^3 curvePoints(:,3).^2 curvePoints(:,3).^1 curvePoints(:, 3).^0];
binx = curvePoints(:, 1);
biny = curvePoints(:, 2);

polyX = transpose(lscov(Ain, binx));  
%y as a function of z
polyY = transpose(lscov(Ain, biny));  
model = [0 0 1 0; polyX; polyY];


%plot 1
frac1 = .7;
nPointsDop = round(npoints*frac1);
distMaxDop = 25;
zzzrange = zrange;
zzzrange(1) = (zrange(1)-zrange(2))*frac1+zzzrange(2);
zzzcoords = ones(nPointsDop,1)*zzzrange(1)+([0:1/(nPointsDop-1):1]')*(zzzrange(2)-zzzrange(1));%unifrnd(zrange(1), zrange(2), npoints, 1);
zzzcoords = [zzzcoords.^3 zzzcoords.^2 zzzcoords.^1 zzzcoords.^0];
coordsDop = [zzzcoords(:,3) zzzcoords*transpose(polyX) zzzcoords*transpose(polyY)];

AinDop = [coordsDop(:,1).^1 coordsDop(:,1).^0];
modelDop = [1 0; transpose(lscov(AinDop, coordsDop(:,2))); transpose(lscov(AinDop,coordsDop(:,3)))];

offsets = unifrnd(-distMaxDop/sqrt(2), distMaxDop/sqrt(2), [nPointsDop 3]);
coordsDop = coordsDop+offsets;%[zeros(size(coords,1),1) offsets];
pPoints = PolyPoints(zzzrange, modelDop, 2000);

subplot(1,3,1);
hold on;
scatter3(coordsDop(:,1),coordsDop(:,2),coordsDop(:,3), 40, [.4, .4, .4], 's','LineWidth',2);
plot3(pPoints(:,1),pPoints(:,2),pPoints(:,3),'--','Color',[.1,.1,.1],'LineWidth',2);
view(2);
camroll(270);
xlim([40 300]);
ylim([30 130]);
grid on;
box on;

XL = get(gca, 'XLim');
YL = get(gca, 'YLim');
set(gca, 'XTick', XL(1):35:XL(2), ...
           'YTick', YL(1):35:YL(2));
set(gca, 'YTickLabel', []);
set(gca, 'XTickLabel', []);
set(gcf, 'color','w');

%plot 2
subplot(1,3,2);
frac2 = 5/6;
nPoints2 = npoints;
nPoints2O = round(nPoints2*outlierFrac);
distMaxDop = 25;
zzrange = zrange;
zzrange(1) = (zrange(1)-zrange(2))*frac2+zzrange(2);

zcoords = ones(npoints,1)*zrange(1)+([0:1/(npoints-1):1]')*(zrange(2)-zrange(1));%unifrnd(zrange(1), zrange(2), npoints, 1);
zcoords = [zcoords.^3 zcoords.^2 zcoords.^1 zcoords.^0];
coords = [zcoords(:,3) zcoords*transpose(polyX) zcoords*transpose(polyY)];

zcoordLast = [zrange(1)^3 zrange(1)^2 zrange(1)^1 zrange(1)^0];
coordLast = [zcoordLast(3) zcoordLast*transpose(polyX) zcoordLast*transpose(polyY)];

offsets = unifrnd(-distMax/sqrt(2), distMax/sqrt(2), [npoints 3]);
coords = coords+offsets;%[zeros(size(coords,1),1) offsets];

% 
% scatter3(mps(oliers,1), mps(oliers,2), mps(oliers,3), 40, [.3, .3, .3],'LineWidth',1);
% scatter3(ptsForModel(:,1),ptsForModel(:,2),ptsForModel(:,3), 40, 'b','LineWidth',2);
scatter3(coords(:,1),coords(:,2), coords(:,3), 40, 'b', 'LineWidth', 2);
hold on;

zcoords2 = ones(nPoints2O,1)*zrange(1)+([0:1/(nPoints2O-1):1]')*(zrange(2)-zrange(1));%unifrnd(zrange(1), zrange(2), npoints, 1);
offsets2 = unifrnd(-distMaxOutlier/sqrt(2), distMaxOutlier/sqrt(2), [nPoints2O 2]);
zcoords2 = [zcoords2.^3 zcoords2.^2 zcoords2.^1 zcoords2.^0];
coords2 = [zcoords2(:,3) zcoords2*transpose(polyX) zcoords2*transpose(polyY)];
coords2 = coords2+[zeros(size(coords2,1),1) offsets2];
scatter3(coords2(:,1),coords2(:,2), coords2(:,3), 40, 'k', 'd', 'LineWidth', 2);

pPoints2 = PolyPoints(zzrange, modelDop, 2000);
plot3(pPoints2(:,1),pPoints2(:,2),pPoints2(:,3),'--','Color',[.1,.1,.1],'LineWidth',2);

pPoints2 = PolyPoints(zzrange, model, 2000);
plot3(pPoints2(:,1),pPoints2(:,2),pPoints2(:,3),'r','LineWidth',2);

scatter3(coordLast(:,1),coordLast(:,2),coordLast(:,3), 40, 'b', 'LineWidth',2);

view(2);
camroll(270);
xlim([40 300]);
ylim([30 130]);
box on;

XL = get(gca, 'XLim');
YL = get(gca, 'YLim');
set(gca, 'XTick', XL(1):35:XL(2), ...
           'YTick', YL(1):35:YL(2));
set(gca, 'YTickLabel', []);
set(gca, 'XTickLabel', []);
set(gcf, 'color','w');


%plot 3

subplot(1,3,3);
% 
% scatter3(mps(oliers,1), mps(oliers,2), mps(oliers,3), 40, [.3, .3, .3],'LineWidth',1);
% scatter3(ptsForModel(:,1),ptsForModel(:,2),ptsForModel(:,3), 40, 'b','LineWidth',2);

hold on;
scatter3(coords(:,1),coords(:,2), coords(:,3), 40, 'b', 'LineWidth', 2);
scatter3(coords2(:,1),coords2(:,2), coords2(:,3), 40, 'k', 'd', 'LineWidth', 2);
pPoints = PolyPoints(zrange, model, 2000);
plot3(pPoints(:,1),pPoints(:,2),pPoints(:,3),'r','LineWidth',2);

scatter3(coordLast(:,1),coordLast(:,2),coordLast(:,3), 40, 'b', 'LineWidth',2);

view(2);
camroll(270);

xlim([40 300]);
ylim([30 130]);

grid on;
box on;

XL = get(gca, 'XLim');
YL = get(gca, 'YLim');
set(gca, 'XTick', XL(1):35:XL(2), ...
           'YTick', YL(1):35:YL(2));
set(gca, 'YTickLabel', []);
set(gca, 'XTickLabel', []);
set(gcf, 'color','w');
