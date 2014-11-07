clear; clc; close all;

load('Jan21/LiverScan/scan3.mat');

FitCurveToLabeledImagePoints;

load('Jan21/LiverScan/scan3_centroids_corrected.mat');
load('Jan21/LiverScan/scan3_output.mat');

fname = 'Y:/NeedleScan/Jan21_LiverScan/Scan 3/scan';

fgps = fopen(strcat(fname, '.gps'), 'r');

ImageWeightExperiment;

close all;

model.model = model;
model.range = range;
model.descriptor = descriptor;

load('Jan21/LiverScan/scan3_centroids_uncorrected.mat');
worldCens = [];
for i=1:size(ipc,1)
    gps = RPreadgpsindex(fgps, gpsheader, ipc(i, 6));
    tempPt = MapImageCoordToWorldCoord(ipc(i, [1 2]), gpsheader, mpp, gps);
    worldCens = [worldCens; transpose(tempPt)];
end
randomVals = [1:50]';%round(rand(50,1)*50);
randomVals = max(randomVals,1);
scatter3(worldCens(randomVals,1), worldCens(randomVals,2), worldCens(randomVals,3), 70, 'b', 'LineWidth', 2);

begRange = oracleModel.range;
tvals = TvalsFromDescriptor(worldCens(randomVals,:), oracleModel.descriptor);
oracleModel.range = [min(tvals) max(tvals)];
pPoints = PolyPoints(oracleModel.range, oracleModel.model, 2000);
hold on;
grid on;
h2 = plot3(pPoints(:,1),pPoints(:,2),pPoints(:,3),'r','LineWidth', 2);
axis equal;
hold on;

rangey = MapRangeFromDescriptors(oracleModel.descriptor, oracleModel.model, oracleModel.range, init.descriptor);
pPoints = PolyPoints(rangey, init.model, 2000);
h1 = plot3(pPoints(:,1),pPoints(:,2),pPoints(:,3),'y','LineWidth', 2);

view(2);
camroll(90);
xlabel('x (mm)');
ylabel('y (mm)');
zlabel('z (mm)');

close all;

pPoints = PolyPoints(model.range, model.model, 2000);
hold on;
grid on;
h2 = plot3(pPoints(:,1),pPoints(:,2),pPoints(:,3),'r','LineWidth', 2);
axis equal;
hold on;

npts = 100;
while(npts > 0)
    idx = round(size(fitPoints,1)*rand(1,1));
    idx = max(idx,1);
    modelScore = EvaluateModelOnPoints(model.model, model.descriptor, degree, fitPoints(idx,:));
    if(modelScore <= minMaxDist)
        continue;
    end
    scatter3(fitPoints(idx,1), fitPoints(idx,2), fitPoints(idx,3), 70, [.5, .5, .5], 'LineWidth', 2);
    npts = npts-1;
end

tvals = TvalsFromDescriptor(ptsForModel, model.descriptor);

npts = 35;
delta = (model.range(2)-model.range(1))/npts;
for i=1:npts
    idealTval = model.range(1)+i*delta;
    [ss idx] = min(abs(tvals-idealTval));
    scatter3(ptsForModel(idx,1), ptsForModel(idx,2), ptsForModel(idx,3), 70, 'b', 'LineWidth', 2); 
end
view(2);
camroll(90);
xlim([80 125]);
ylim([81 123]);
xlabel('x (mm)');
ylabel('y (mm)');
zlabel('z (mm)');





