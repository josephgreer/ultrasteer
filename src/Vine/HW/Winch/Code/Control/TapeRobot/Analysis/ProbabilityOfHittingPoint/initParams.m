function params = initParams()
params = [];

params.gridWidth = 10;
params.gridHeight = 10;
params.radStep = deg2rad(30);
params.nGridPoints = 30;

params.widthStep = params.gridWidth/(params.nGridPoints-1);
params.heightStep = params.gridHeight/(params.nGridPoints-1);
params.nRadPoints = round(2*pi/params.radStep+1);

pSuccess = 0.5;
pFail = (1-pSuccess)/2;
params.Pturns = [pSuccess pFail pFail; pFail pSuccess pFail; pFail pFail pSuccess];

params.deadBandRad = deg2rad(80);
params.actuatorSpacing = params.gridWidth/10;
params.nActuators = 10;
end