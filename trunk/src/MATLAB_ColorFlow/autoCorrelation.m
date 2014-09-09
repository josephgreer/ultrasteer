function [velocity, sigma, power] = autoCorrelation(I, Q, ensemble)
% The following script perfroms autocorrelation on I/Q data to estimate the
% Dopper frequency.
%
% Copyright Ultrasonix Medical corporation
% Author: Reza Zahiri Azar - July 2010

nRy = 0; nRx = 0; nR0 = 0;  nRt = 0; nRt2 = 0;                  
for i = 1:ensemble-1
    % correlation with one lag
    Icurrent = I(i);   
    Inext = I(i+1);
    Qcurrent = Q(i);
    Qnext = Q(i+1);
					
    nRy	= nRy + Icurrent * Qnext - Qcurrent * Inext;        % real part of autocorrelation
    nRx	= nRx + Icurrent * Inext + Qcurrent * Qnext;        % imaginary part of autocorrelation
    nR0 = nR0 + Icurrent * Icurrent + Qcurrent * Qcurrent;  % power of signal (Power Doppler)
end;
nR0 = nR0 + Inext * Inext + Qnext * Qnext; % update the power Doppler with lase values
		
%compute velocity from the phase
velocity = atan2(nRy,nRx);

% compute sigma (variance)
nRt = sqrt( nRx * nRx + nRy * nRy );
sigma = ( 1 - nRt/nR0 );	    
        
% computer power Doppler in dB
power = 20*log10(nR0);
