% $Id: EllipsePointsFromParam.m,v 1.1 2007/01/10 03:21:31 anton Exp $

function [ellipsePoints] = EllipsePointsFromParam(ellipseParam)
   
    cos_phi = cos(ellipseParam.phi);
    sin_phi = sin(ellipseParam.phi);
    
    % rotation matrix to rotate the axes with respect to an angle phi
    R = [ cos_phi sin_phi; -sin_phi cos_phi ];
    
    % the axes
    ver_line        = [[ellipseParam.X0 ellipseParam.X0]; ellipseParam.Y0 + ellipseParam.b * [-1 1] ];
    horz_line       = [ellipseParam.X0 + ellipseParam.a * [-1 1]; [ellipseParam.Y0 ellipseParam.Y0] ];
    new_ver_line    = R * ver_line;
    new_horz_line   = R * horz_line;
    
    % the ellipse
    theta_r         = linspace(0, 2 * pi);
    ellipse_x_r     = ellipseParam.X0 + ellipseParam.a * cos(theta_r);
    ellipse_y_r     = ellipseParam.Y0 + ellipseParam.b * sin(theta_r);
    ellipsePoints = R * [ellipse_x_r; ellipse_y_r];


% $Log: EllipsePointsFromParam.m,v $
% Revision 1.1  2007/01/10 03:21:31  anton
% Segmentation: Added separate functions to handles ellipses
%
%
