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
%   This function will call the ls3dline algorithm by sending the
%   ROImatrix, it will then call Linepts to generate X,Y,Z points that
%   correspond to that line.
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: FitLine()
% =========================================================================

function [Xpts,Ypts,Zpts,x1, a1,d1,normd1] = FitLine(ROImatrix)

% -------------------------------------------------------------------------
% Initialization of variables
% -------------------------------------------------------------------------

x0 = [mean(ROImatrix(:,1));mean(ROImatrix(:,2));mean(ROImatrix(:,3))];
a0 = [1;1;1];
r0 = 100; % orig 100
tolp = 0.1; % orig 10
tolg = 0.1; % orig 10
d1 = [];
normd1 = [];
x1 = [];
a1 = [];

% -------------------------------------------------------------------------
% Call 3D Least Squares Fitting Function
% -------------------------------------------------------------------------

 [x1, a1, d1, normd1] = ls3dline(ROImatrix);
 [Xpts,Ypts,Zpts] = Linepts(x1,a1);

end

% =========================================================================
% END FUNCTION: FitCurve()
% =========================================================================