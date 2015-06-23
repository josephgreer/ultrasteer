function minMeas = getMinimumMeasurements(params)
if(params.particleFilterMethod == 1)
    minMeas = params.p1.minimumMeasurements;
elseif(params.particleFilterMethod == 2)
    minMeas = params.p2.minimumMeasurements;
elseif(params.particleFilterMethod == 3)
    minMeas = params.p3.minimumMeasurements;
elseif(params.particleFilterMethod == 4)
    minMeas = params.p4.minimumMeasurements;
else
    minMeas = params.p100.minimumMeasurements;
end
end