f = strcat(fname, '.gps1');
fgps = fopen(f, 'r');
f = strcat(fname, '.b8');
fim = fopen(f, 'r');

mpp = 83;
gpsheader = ReadHeader(fgps);
imheader = ReadHeader(fim);
nframes = max(ips(:,6));

points = [];
indices = [];


% frame indices to calibrate initial model from
% calibrationIndex = [10 100];
% fitPoints = [];
% for i=1:size(calibrationIndex,2)
%     im = RPreadframeindex(fim, imheader, calibrationIndex(i));
%     gps = RPreadgpsindex(fgps, gpsheader, calibrationIndex(i));
%     figure(1);
%     imshow(im, [0 255]);
%     fitPoints = [fitPoints; transpose(MapImageCoordToWorldCoord(ginput(1), gpsheader, mpp, gps))];
% end
% figure(1);
% close;

imageWeights = [];
modelWeights = [];

fitPoints = [];
for i=1:initFrames
    fitPoints = [fitPoints; transpose(MapImageCoordToWorldCoord(ipc(i, [1:2]), gpsheader, mpp, RPreadgpsindex(fgps, gpsheader, ipc(i,6))))];
end
figure(25);
%scatter3(fitPoints(:,1), fitPoints(:,2), fitPoints(:,3), 'r');
hold on;

descriptor.dir = transpose(fitPoints(size(fitPoints,1),:)-fitPoints(1,:));  %don't point(last)-point(first) beause this messes up range for some reason
descriptor.offset = transpose(fitPoints(1,:));
descriptor.dir = descriptor.dir/norm(descriptor.dir);
model = FitPoly(fitPoints, 1, descriptor);
model = [zeros(3, degree+1-2) model];

init.model = model;
init.descriptor = descriptor;

fitPoints = [];
for i=1:nframes
    if length(find(excludeFrames == i) > 0)
        continue;
    end
    
    gps = RPreadgpsindex(fgps, gpsheader, i);
    fps = find(ips(:,6) == i);
    for j=1:size(fps,1)
        fitPoints = [fitPoints; transpose(MapImageCoordToWorldCoord(ips(fps(j),[1:2]), gpsheader, mpp, gps))];
    end
end

%scatter3(fitPoints(:,1), fitPoints(:,2), fitPoints(:,3));
%hold on;
pPoints = PolyPoints(oracleModel.range, oracleModel.model, 2000);
hold on;
hOracle = plot3(pPoints(:,1),pPoints(:,2),pPoints(:,3),'r','LineWidth', 2);
axis equal;


mps = [];
currModel = [];
maxDist = initMaxDist;
dopplerResp = [];
usedIndices = [];
for i=1:nframes
    fps = ips(find(ips(:,6) == i),:);
    if(size(fps,1) < 1)
        continue;
    end;
    
    gps = RPreadgpsindex(fgps, gpsheader, i);
    
    for j=1:size(fps,1)
        wpt = transpose(MapImageCoordToWorldCoord(fps(j,[1:2]), gpsheader, mpp, gps));
        mps = [mps;  wpt fps(j,[3:size(fps,2)])];
    end
    
    modelScores = EvaluateModelOnPoints(model, descriptor, degree, mps(:,[1:3]));
    imageScores = mps(:,4);
    weights = modelWeight*(1./max(modelScores,1))+imageWeight*imageScores;
    
    ptsForModel = [];
    usedIndices = [];
    fidx = size(mps,2); % frame index is last column in model points
    for j=1:max(mps(:,fidx))
        %%%%% ids of current model points that came from current frame
        cptIdx = find(mps(:,size(mps,2)) == j);
        if(size(cptIdx,1) == 0)
            continue;
        end
        
        %%%%% find the minimum index model point for this frame
        fakeScores = ones(size(modelScores,1),1)*1000000;
        fakeScores(cptIdx,:) = modelScores(cptIdx,:);
        [ss indices] = min(fakeScores);
        if(modelScores(indices(1)) > maxDist)
            continue;
        end;
        
        
        %%%%% TODO:  Should there be a threshold on distance from current
        %%%%% model to add to ptsForModel?
        %%%%%% choose the smallest model point and add to the ptsForModel 
        ptsForModel = [ptsForModel; mps(indices(1),[1:3])];
        usedIndices = [usedIndices; indices(1)];
    end
    
    if(size(ptsForModel,1)<minPoints)
        continue;
    end
    
    maxDist = initMaxDist-min((size(ptsForModel,1)-20)/75,1)*(initMaxDist-minMaxDist);
    
    tvals = TvalsFromDescriptor(ptsForModel, descriptor);
    rng = [min(tvals) max(tvals)];
    endPoints = ModelEndPoints(model, rng, descriptor);
    descriptor.dir = (endPoints(:,2)-endPoints(:,1));
    descriptor.dir = descriptor.dir/norm(descriptor.dir);
    descriptor.offset = endPoints(:,1);
    
    model = FitPoly(ptsForModel, degree, descriptor);

    range = MapRangeFromDescriptors(oracleModel.descriptor, oracleModel.model, oracleModel.range, descriptor);
    
    
    %model = FitPolyWeighted(mps, weights, degree, descriptor);
    
    modelWeight = min(modelWeight*(1+modelGrowthRate),maxModelWeight);
end
hiscatter = scatter3(ptsForModel(:,1), ptsForModel(:,2), ptsForModel(:,3), 60, 'b', 'LineWidth',2);

centroidCoords = [];
scores = [];
%usedIndices = transpose([1:max(ipc(:,6))]);
mscores = EvaluateModelOnPoints(model, descriptor, degree, ptsForModel(:,[1:3]));
for i=1:size(usedIndices,1)
    idx = usedIndices(i);
    currscore = ips(idx,4);%(ips(idxs,4)+ips(idxs,5))/ips(idxs,6);
    scores = [scores; mscores(i)];
    centroidCoords = [centroidCoords; transpose(MapImageCoordToWorldCoord(ips(idx,[1:2]), gpsheader, mpp, RPreadgpsindex(fgps, gpsheader, ips(idx,6))))];
end
tvals = TvalsFromDescriptor(centroidCoords, descriptor);
range = [min(tvals) max(tvals)];
set(datacursormode, 'UpdateFcn', @(obj,event_obj)(DataCursorIndex(obj,event_obj,usedIndices, ptsForModel)));
pPoints = PolyPoints(range, model, 2000);
hNeedle = plot3(pPoints(:,1),pPoints(:,2),pPoints(:,3),'k','LineWidth',2);

figure(26);
[soted idxs] = sort(tvals);
sortedScores = scores(idxs);
sortedTvals = tvals(idxs);
scatter(sortedTvals, sortedScores);

% summedDopplerResp = [];
% for i=2:size(sortedDopplerResp,1)
%     idxs = i;%find(abs(sortedTvals-sortedTvals(i)) < .1);
%     numerator = sum(sortedDopplerResp(idxs));
%     %denominator = (sortedTvals(i)-sortedTvals(i-1));
%     %denominator = denominator^2;
%     denominator = 1;
%     summedDopplerResp = [summedDopplerResp; numerator/denominator]; 
% end
% figure(27);
% scatter(sortedTvals([2:size(sortedTvals,1)]), summedDopplerResp);


rangeDelta = (oracleModel.range(2)-oracleModel.range(1))/10;
rmsErrorEnd = CompareModels(oracleModel.model, oracleModel.descriptor, [oracleModel.range(2)-rangeDelta oracleModel.range(2)], model, descriptor)

rmsErrorBeg = CompareModels(oracleModel.model, oracleModel.descriptor, [oracleModel.range(1) oracleModel.range(1)+rangeDelta], model, descriptor)
rmsErrorTotal = CompareModels(oracleModel.model, oracleModel.descriptor, oracleModel.range, model, descriptor)
