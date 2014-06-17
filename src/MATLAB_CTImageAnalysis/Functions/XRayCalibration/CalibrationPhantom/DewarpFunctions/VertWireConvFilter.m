function f = VertWireConvFilter(filterSize, darkOnBright)
% function f = VertWireConvFilter(filterSize, darkOnBright)
%
% This function returns a convolution filter for detecting a "vertical
% wire" pattern in an image.  The returned filter may detect a dark line on
% a bright background or a bright line on a dark background, depending on
% the `darkOnBright' parameter.  Typically, after convolving an image with
% this filter, the  line responses will be more than 0.625 the maximum
% response. 
%
f = ones(filterSize);

midMask = floor((size(f,2) + 1) / 2);
f(:, [(midMask-2) : (midMask+2)]) = 0.5;
f(:, [(midMask-1) : (midMask+1)]) = 0;
f(:, midMask) = -1;

if (~darkOnBright)
    f = 1 - f;
end

f = f - mean(mean(f)) - (1 / prod(filterSize));