close all;
clearvars -except fitPoints;
clc;
fname = 'Y:/NeedleScan/Jan21_LiverScan/Scan 3/scan';
f = strcat(fname, '.gps1');
fgps = fopen(f, 'r');
f = strcat(fname, '.b8');
fim = fopen(f, 'r');

scatter3(fitPoints(:,1), fitPoints(:,2), fitPoints(:,3));


mpp = 83;
gpsheader = ReadHeader(fgps);
imheader = ReadHeader(fim);

degree = 3;             %% polynomial degree to fit to needle
descriptor.dir = [1; 0; 0];         %% descriptor variable
descriptor.offset = [0; 0; 0];
minFrames = 10;         %% minimum frames to start fitting a model to
maxModelWeight = 100;       %% point weight = modelWeight*modelScore+imageWeight*imageScore
modelGrowthRate = .1;
modelWeight = 10;
imageWeight = 1;

nframes = max(fitPoints(:,5));


points = [];
indices = [];


% frame indices to calibrate initial model from
calibrationIndex = [10 100];
calPoints = [];
for i=1:size(calibrationIndex,2)
    im = RPreadframeindex(fim, imheader, calibrationIndex(i));
    gps = RPreadgpsindex(fgps, gpsheader, calibrationIndex(i));
    figure(1);
    imshow(im, [0 255]);
    calPoints = [calPoints; transpose(MapImageCoordToWorldCoord(ginput(1), gpsheader, mpp, gps))];
end
figure(1);
close;

imageWeights = [];
modelWeights = [];

%beginning model
model = FitPoly(calPoints, 1, descriptor);
model = [zeros(3, degree+1-2) model];

mps = [];
for i=1:nframes
    fp = fitPoints(find(fitPoints(:,5) == i),:);
    
    mps = [mps; fp];
    
    if(i<minFrames)
        continue;
    end
        
    modelScores = EvaluateModelOnPoints(model, descriptor, degree, mps(:,[1:3]));
    imageScores = mps(:,4);
    weights = modelWeight*(1./max(modelScores,1))+imageWeight*imageScores;
    
    model = FitPolyWeighted(mps, weights, degree, descriptor);
    
    modelWeight = min(modelWeight*(1+modelGrowthRate),maxModelWeight);
end


