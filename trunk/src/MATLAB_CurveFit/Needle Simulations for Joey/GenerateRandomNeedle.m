% Generate a random needle from parameters
% curvarature = needle curvature in mm
% nBends = number of times needle changes direction
% nMeasurements = number of measurements
% length = needle length in mm
function needle = GenerateRandomNeedle(nMeasurements, length, curvature, nBends)
    cycleMod = nMeasurements/nBends;
    increment = length/nMeasurements;
    u = [0 curvature increment];
    x(:, :, 1) = [0 0 0 0 0 0]';
    for i=1:nMeasurements-1
        if(mod(i, cycleMod) == 0)
            u(1) = randi(10, 1);
        else
            u(1) = 0;
        end
        x(:,:,i+1) = f(x(:,:,i),u,zeros(6, 1));
    end
    needle = x([1:3],:,:);
    needle = x(:, 1, :);
    needle = transpose(needle);
end