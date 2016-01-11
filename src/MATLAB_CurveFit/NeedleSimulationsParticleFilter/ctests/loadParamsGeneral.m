function [params, worked] = loadParamsGeneral(params, splits)
worked = 0;
if(strcmpi(splits{1}, 'n'))
    params.n = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'neff'))
    params.neff = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'mpp'))
    params.mpp = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'tipOffset'))
    params.tipOffset = [str2num(splits{2}); str2num(splits{3}); str2num(splits{4})];
    worked = 1;
elseif(strcmpi(splits{1}, 'lutBasePath'))
    params.LUTDistributionBasePath = splits{2};
    worked = 1;
elseif(strcmpi(splits{1}, 'useLUT'))
    params.useLUTDistribution = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'usw'))
    params.usw = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'ush'))
    params.ush = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'dopplerLambda'))
    params.lambdaDop = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'dopplerSigB0'))
    params.sigB0 = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'dopplerSigB1'))
    params.sigB1 = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'offFrameB0'))
    params.offFrameB0 = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'offFrameB1'))
    params.offFrameB1 = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'measurementOffsetSigma'))
    params.measurementOffsetSigma = diag([str2num(splits{2}) str2num(splits{3})]);
    worked = 1;
elseif(strcmpi(splits{1}, 'particleSigmaVel'))
    params.sigmaVelocity = str2num(splits{2});
    worked = 1;
elseif(strcmpi(splits{1}, 'particleSigmaOr'))
    params.sigmaOrientation = diag([str2num(splits{2}) str2num(splits{3}) str2num(splits{4})]);
    worked = 1;
elseif(strcmpi(splits{1}, 'particleSigmaRho'))
    params.sigmaRho = str2num(splits{2});
    worked = 1;
end
end