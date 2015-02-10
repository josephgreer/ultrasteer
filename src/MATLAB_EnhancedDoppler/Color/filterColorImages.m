function [vel, pow, sig] = filterColorImages(vel, pow, sig, ~, powC, ~)
% The following script filtered the velocity, power, and sigma images based
% on the predefined threshold values.
%
% Copyright Ultrasonix Medical corporation - Analogic Ultrasound Group
% Author: Reza Zahiri Azar - Jan 2014

% filter settings, these values need to be optimized for different settings
powerMin = 20;   % minimum required power to be considered in dB
powerMaxClutter = 120; % maximum clutter power allowed in dB

% set to zero
vel(  pow< powerMin  ) = 0;
vel(  powC> powerMaxClutter  ) = 0;

pow(  pow< powerMin  ) = 0;
pow(  powC> powerMaxClutter  ) = 0;

sig(  pow< powerMin  ) = 0;
sig(  powC> powerMaxClutter  ) = 0;

% spatial filtering remove outliers
vel = medfilt2(vel);
sig = medfilt2(sig);
pow = medfilt2(pow);
