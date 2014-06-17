% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: FitLine(ROImatrix)
% -------------------------------------------------------------------------
% Inputs: ROImatrix (x,y,z) - matrix of 3D points in the region of interest
% Outputs: Xpts, Ypts, Zpts, - points on the line, x1 - centroid of data,
% a1 - direction cosines, d1 - residuals, normd1 - norm of residual errors.
% Sub_Functions Called: ls3dline()
% Synopsis:
%   This function will generate points on a line 75mm on both sides of the
%   center point of the needle x0 using the direction cosines a. 
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: Linepts()
% =========================================================================

function [Xpts,Ypts,Zpts] = Linepts(x0,a)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

extra = 75;

Xpts = [(x0(1)-extra*a(1)),x0(1),(x0(1)+extra*a(1))];
Ypts = [(x0(2)-extra*a(2)),x0(2),(x0(2)+extra*a(2))];
Zpts = [(x0(3)-extra*a(3)),x0(3),(x0(3)+extra*a(3))];

end

% =========================================================================
% End FUNCTION: Linepts()
% =========================================================================
