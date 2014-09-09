function [vel, pow, sig] = filterColorImages(vel, pow, sig, velC, powC, sigC)
% The following script filtered the velocity, power, and sigma images based
% on the predefined threshold values. The incput data are the output of
% autocorrlation function of both cluttered IQ and wall filtered IQ
%
% Copyright Ultrasonix Medical corporation
% Author: Reza Zahiri Azar - July 2010

% Filtering values
% Note: this values need to be optimized for different settings
sigmaMax = 0.55;
powerMin = 70;   % dB
powerMax = 140;  % dB
powerMaxClutter = 200; %dB
powerMinNoise = (powerMin + powerMax)/2; velMaxNoise = 2*pi/3;

% set to zero
vel( find( sig> sigmaMax ) ) = 0;
vel( find( pow> powerMax ) ) = 0;
vel( find( pow< powerMin ) ) = 0;
vel( find( powC> powerMaxClutter ) ) = 0;
vel( find( (pow < powerMinNoise) & ( abs(vel)> velMaxNoise) ) ) = 0;

pow( find( sig> sigmaMax ) ) = 0;
pow( find( pow> powerMax ) ) = 0;
pow( find( pow< powerMin ) ) = 0;
pow( find( powC> powerMaxClutter ) ) = 0;

sig( find( sig> sigmaMax ) ) = 0;
sig( find( pow> powerMax ) ) = 0;
sig( find( pow< powerMin ) ) = 0;
sig( find( powC> powerMaxClutter ) ) = 0;

% remove outliers
vel = medfilt2(vel);
sig = medfilt2(sig);
pow = medfilt2(pow);

% upsample
vel = interp2(vel,1);
sig = interp2(sig,1);
pow = interp2(pow,1);