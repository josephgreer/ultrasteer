function [xMatrix, yMatrix] = ExtractBernsteinTransformCoefficients(TFORMArray, plotCoefficients, figIdx)

numFrames = length(TFORMArray);

xMatrix = [];
yMatrix = [];

for m=1:numFrames
    TFORM = TFORMArray(m);
    xMatrix = [xMatrix, TFORM.tdata.coefficientsX];
    yMatrix = [yMatrix, TFORM.tdata.coefficientsY];
end

if (~plotCoefficients)
    return
end

if (exist('figIdx'))
    figure(figIdx);
end

subplot(2, 1, 1);
plot(xMatrix');
subplot(2, 1, 2);
plot(yMatrix');
