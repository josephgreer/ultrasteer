clear; 
clc;
close all;

%%%% parameters
nout = .9;  %outlier percent
curvePoints = [51 -10 311; 51 0 241; -77 15 107; 132 20 45];  %oracle polynomial points
curveScale = 1/10;   
nmeasurement = 5000;
nIts = 168;

%inlier measurement description
inlierImageScoreCorrelation = -.5;  % how well is our image score correlated with distance
inlierMeasurementError = 3*curveScale;
inlierScoreMean = .8;

%outlier measurement description
normallyDistributedOutliers = false;  %draw outliers from normal distribution or uniform distribution?
outlierScoreMean = .5;
outlierMeasurementError = 25*curveScale;

%%%% end parameters

addpath('.\Needle Simulations for Joey');

%seed rand for deterministic results!
rng(1);


%generate the oracle curve from curvePoints 
curvePoints = curvePoints*curveScale;

Ain = [curvePoints(:, 3).^3 curvePoints(:,3).^2 curvePoints(:,3).^1 curvePoints(:, 3).^0];
binx = curvePoints(:, 1);
biny = curvePoints(:, 2);
win = ones(size(binx));

% use z as a descriptor variable and regress x and y off z
zrange = [min(curvePoints(:,3)) max(curvePoints(:, 3))];
Zplot = transpose([min(curvePoints(:, 3)):.5:max(curvePoints(:,3))]);
Zcoeffs = [Zplot.^3 Zplot.^2 Zplot.^1 Zplot.^0];

%x as a function of z
polyX = lscov(Ain, binx);  
%y as a function of z
polyY = lscov(Ain, biny);  

figure;
set(figure(1),'name','measurement plot','numbertitle','off');
hold on;
scatter3(curvePoints(:, 3), curvePoints(:, 1), curvePoints(:, 2));

oracle = [Zplot Zcoeffs*polyX Zcoeffs*polyY];
plot3(oracle(:,1), oracle(:,2), oracle(:,3));
axis equal;
xlabel('z');
ylabel('x');
zlabel('y');
norm(curvePoints(4,:)-curvePoints(1,:));


% now we have the "oracle" model, given by polyX and polyY which describes X and Y
% as functions of Z

%describe measurement score random variable for inliers
upperScaleFactorMeasurement = 2;
inlierMeasurementVariance = (inlierMeasurementError/3)^2;     % 3 standard deviations of mean.  98% will be above 0
inlierScoreVariance = ((inlierScoreMean/4)/3)^2;  %3 standard deviations within scoreMean/4
distScore = GenerateErrorScore(inlierImageScoreCorrelation, inlierMeasurementError, upperScaleFactorMeasurement, inlierMeasurementVariance, inlierScoreMean, inlierScoreVariance, nmeasurement);
inliers = GenerateSamplesFromErrorDistScore(distScore, zrange, polyX, polyY, nmeasurement);
inlierImageScores = max(distScore(:,2),0);              % should be a low number but is possible

if(normallyDistributedOutliers)
    %describe measurement score random variable for outliers
    ouliterImageScoreCorrelation = -.5;
    upperScaleFactorMeasurmement = 2;
    outlierMeasurementVariance = (outlierMeasurementError/3)^2;     % 3 standard deviations of mean.  98% will be above 0
    outlierScoreVariance = (outlierScoreMean/3)^2;
    distScoreOutliers = GenerateErrorScore(ouliterImageScoreCorrelation, outlierMeasurementError, upperScaleFactorMeasurement, outlierMeasurementVariance, outlierScoreMean, outlierScoreVariance, nmeasurement);
    outliers = GenerateSamplesFromErrorDistScore(distScoreOutliers, zrange, polyX, polyY, nmeasurement);
    outlierImageScores = max(distScoreOutliers(:,2), 0);
    figure(3);
    scatter(distScoreOutliers(:,1), distScoreOutliers(:,2));
    set(figure(3),'name','outlier scatter','numbertitle','off');
else
    oliers = GenerateOutliers(outlierScoreMean, (outlierScoreMean/3)^2, outlierMeasurementError, nmeasurement, zrange, polyX, polyY);
    outliers = oliers(:,[1:3]);
    outlierImageScores = max(oliers(:, 4), 0);
end


figure(2);
scatter(distScore(:,1), distScore(:,2));
set(figure(2),'name','inlier scatter','numbertitle','off');

figure(1);

ninsDis = round(500*(1-nout));
noutsDis = 500-ninsDis;
scatter3(inliers([1:ninsDis],1), inliers([1:ninsDis],2), inliers([1:ninsDis],3), 'r.');
scatter3(outliers([1:noutsDis],1), outliers([1:noutsDis],2), outliers([1:noutsDis],3), 'kx');

nstart = 500;
nins = round(nstart*(1-nout));
nouts = nstart-nins;
iin = inliers([1:nins],:);
inliers = removerows(inliers, 'ind', [1:nins]); 
oin = outliers([1:nouts], :);
outliers = removerows(outliers, 'ind', [1:nouts]); 

measurements = [iin; oin];
imageScores = [inlierImageScores([1:nins]); outlierImageScores([1:nouts])];
inlierImageScores = removerows(inlierImageScores, 'ind', [1:nins]);
outlierImageScores = removerows(outlierImageScores, 'ind', [1:nouts]);

modelScores = ones(length(imageScores), 1);
model = FitModel(measurements, imageScores);
polyFit = [Zplot Zcoeffs*model(:,1) Zcoeffs*model(:,2)];
error = RmsError(polyFit, oracle);

errorNull = error;

nPerIt = round(nmeasurement/nIts);
nLeft = nmeasurement-nstart;
rmeRange = [1:size(Zplot, 1)/5];
while(nLeft > 0) 
    inout = unifrnd(1, 100, nPerIt, 1);
    numin = size(inout(inout(:,1) > 100*nout), 1);
    numout = size(inout(inout(:,1) <= 100*nout), 1);
    %inout(:,1) means inlier, else outlier
    
    measurements = [measurements; inliers([1:numin],:)];
    measurements = [measurements; outliers([1:numout],:)];
    imageScores = [imageScores; inlierImageScores([1:numin])];
    imageScores = [imageScores; outlierImageScores([1:numout])];
    
    % remove the measurements from the stack
    inliers = removerows(inliers, 'ind', [1:numin]);
    outliers = removerows(outliers, 'ind', [1:numout]);
    inlierImageScores = removerows(inlierImageScores, 'ind', [1:numin]);
    outlierImageScores = removerows(outlierImageScores, 'ind', [1:numout]);
    
    modelScores = EvaluateModelOnPoints(model(:,1), model(:,2), measurements);
    model = FitModel(measurements, 1./(max(modelScores,1)));
    polyFit = [Zplot Zcoeffs*model(:,1) Zcoeffs*model(:,2)];
    
    modelNull = FitModel(measurements, imageScores);
    polyFitNull = [Zplot Zcoeffs*modelNull(:,1) Zcoeffs*modelNull(:,2)];
    
    rme = RmsError(polyFit(rmeRange,:), oracle(rmeRange,:));
    error = [error; rme];
    
    rme = RmsError(polyFitNull(rmeRange,:), oracle(rmeRange,:));
    errorNull = [errorNull; rme];
    
    nLeft = nLeft-nPerIt;
end

modelNull = FitModel(measurements, ones(size(measurements, 1), 1));
polyFitNull = [Zplot Zcoeffs*modelNull(:,1) Zcoeffs*modelNull(:,2)];
rmsErrorNull = RmsError(polyFitNull, oracle);

figure(5);
set(figure(5),'name','oracle vs fits','numbertitle','off');
hold on;
plot3(oracle(:,1), oracle(:, 2), oracle(:,3), 'b');
plot3(polyFit(:,1), polyFit(:, 2), polyFit(:,3), 'g-');
plot3(polyFitNull(:,1), polyFitNull(:, 2), polyFitNull(:,3), 'y-');
axis equal;
xlabel('z');
ylabel('x');
zlabel('y');
legend('Oracle', 'With Model Weighting', 'Without Model Weighting');

figure(4);
set(figure(4),'name','rms error plots','numbertitle','off');
hold on;
plot(error);
plot(errorNull, 'r');
legend('With Model Weighting', 'Without Model Weighting');
xlabel('Iterations');
ylabel('RmsError');







