function params = loadParamsMarginalized(params, splits)
if(strcmpi(splits{1}, 'initPosSigma'))
    params.p3.initPosSigma = diag([str2num(splits{2}) str2num(splits{3}) str2num(splits{4})]);
elseif(strcmpi(splits{1}, 'minimumMeasurements'))
    params.p1.minimumMeasurements = str2num(splits{2});
elseif(strcmpi(splits{1}, 'initOrientationSigma'))
    params.p3.initOrientationSigma = diag([str2num(splits{2}) str2num(splits{3}) str2num(splits{4})]);
elseif(strcmpi(splits{1}, 'initRhoSigma'))
    params.p3.initRhoSigma = str2num(splits{2});
elseif(strcmpi(splits{1}, 'particleSigmaPos'))
    params.p3.particleSigmaPos = diag([str2num(splits{2}) str2num(splits{3}) str2num(splits{4})]);
elseif(strcmpi(splits{1}, 'measurementSigma'));
    params.p3.measurementSigma = diag([str2num(splits{2}) str2num(splits{3}) str2num(splits{4})]);
elseif(strcmpi(splits{1}, 'subsetSize'));
    params.p3.subsetSize = str2num(splits{2});
elseif(strcmpi(splits{1}, 'procrustesIt'));
    params.p3.procrustesit = str2num(splits{2});
elseif(strcmpi(splits{1}, 'distanceThres'))
    params.p3.distanceThresh = str2num(splits{2});
elseif(strcmpi(splits{1}, 'nPoints'))
    params.p3.nPoints = str2num(splits{2});
end
end