% $Id: EllipseEqFromPoints.m,v 1.2 2007/01/10 17:20:14 anton Exp $

function [ellipseEq] = EllipseEqFromPoints(x, y)

    % prepare vectors, must be column vectors
    x = x(:);
    y = y(:);

    xForEQ = x;
    yForEQ = y;
    AForEQ = [xForEQ.*xForEQ xForEQ.*yForEQ yForEQ.*yForEQ xForEQ yForEQ];
    nForEQ = cond(AForEQ);
    aForEQ = mldivide(AForEQ, ones(size(xForEQ,1), 1));
    ellipseEq = [aForEQ ; -1]';

% $Log: EllipseEqFromPoints.m,v $
% Revision 1.2  2007/01/10 17:20:14  anton
% EllipseEqFromPoints.m: Layout
%
% Revision 1.1  2007/01/10 03:21:31  anton
% Segmentation: Added separate functions to handles ellipses
%
% 
