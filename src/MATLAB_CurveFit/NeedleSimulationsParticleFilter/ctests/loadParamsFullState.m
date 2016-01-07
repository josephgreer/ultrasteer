function params = loadParamsFullState(params, splits)
if(strcmpi(splits{1}, 'initPosSigma'))
    params.p1.initPosSigma = diag([str2num(splits{2}) str2num(splits{3}) str2num(splits{4})]);
elseif(strcmpi(splits{1}, 'minimumMeasurements'))
    params.p1.minimumMeasurements = str2num(splits{2});
elseif(strcmpi(splits{1}, 'initOrientationSigma'))
    params.p1.initOrientationSigma = diag([str2num(splits{2}) str2num(splits{3}) str2num(splits{4})]);
elseif(strcmpi(splits{1}, 'initRhoSigma'))
    params.p1.initRhoSigma = str2num(splits{2});
elseif(strcmpi(splits{1}, 'particleSigmaPos'))
    params.p1.particleSigmaPos = diag([str2num(splits{2}) str2num(splits{3}) str2num(splits{4})]);
end
end