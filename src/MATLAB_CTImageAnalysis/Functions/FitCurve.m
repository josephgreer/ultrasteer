% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: FitCurve(ROImatrix)
% -------------------------------------------------------------------------
% Inputs: ROImatrix (x,y,z) - matrix of 3D points in the region of interest
% Outputs: x0n - circle center,an - normal to circle,rn - radius of circle,
% conv - if the least squares algorithm converged, d - vector of residual 
% errors, sigmah - norm of d divided by DOF, ie. RMSE.
% Sub_Functions Called: ls3dcircle()
% Synopsis:
%   This function will call the ls3dcircle algorithm by sending intial
%   variables for x0 as the center of the ROImatrix, a0, vector of ones, r0
%   100mm radius, and 0.1mm tolerances. ??
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: FitCurve()
% =========================================================================

function [x0n,an,rn,conv,d,sigmah] = FitCurve(ROImatrix)

% -------------------------------------------------------------------------
% Initialization of variables
% -------------------------------------------------------------------------

x0 = [mean(ROImatrix(:,1));mean(ROImatrix(:,2));mean(ROImatrix(:,3))];
a0 = [1;1;1];
r0 = 100; % orig 100
tolp = 0.1; % orig 10
tolg = 0.1; % orig 10

%r0 = 500;

% -------------------------------------------------------------------------
% Call 3D Least Squares Fitting Function
% -------------------------------------------------------------------------

[x0n, an, rn, d, e, f, sigmah, conv, Vx0n, Van, urn, GNlog, a,R0, R] = ls3dcircle(ROImatrix, x0, a0, r0, tolp, tolg);

end

% =========================================================================
% END FUNCTION: FitCurve()
% =========================================================================