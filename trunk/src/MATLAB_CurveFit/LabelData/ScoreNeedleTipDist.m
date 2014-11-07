maxTval = max(sortedTvals);
maxTval = maxTval.^transpose([degree:-1:0]);
needlePointAuto = model*maxTval;

[validPtsIndices validPts] = find(points(:,3) > 0);
labeledPts = [];
for i=1:size(validPtsIndices,1)
    labeledPts = [labeledPts; transpose(MapImageCoordToWorldCoord(points(validPtsIndices(i),[1 2]), gpsheader, mpp, RPreadgpsindex(fgps, gpsheader, validPtsIndices(i))))];
end

ltvals = TvalsFromDescriptor(labeledPts, oracleModel.descriptor);
if(useMin)
    [maxPt maxPtIdx] = min(ltvals);
else
    [maxPt maxPtIdx] = max(ltvals);
end
needlePtManual = labeledPts(maxPtIdx,:);
manualTval = TvalsFromDescriptor(needlePtManual, oracleModel.descriptor);
manualTval = manualTval.^transpose([degree:-1:0]);
needlePointManual = oracleModel.model*manualTval;

needlePointDist = norm(transpose(needlePtManual)-needlePointAuto)

ptDists = EvaluateModelOnPoints(model, descriptor, degree, labeledPts);
meanErrorsLabeled = mean(ptDists);
sigmaErrorsLabeled = std(ptDists);
