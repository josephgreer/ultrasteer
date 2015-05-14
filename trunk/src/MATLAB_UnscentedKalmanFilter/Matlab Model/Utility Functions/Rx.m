function [ R ] = Rx( gamma )
%ROTX Creates a rotation matrix for a rotation around x.
%   [ R ] = rotx( gamma )
% Inputs:   gamma: rotation around x-axis (radians)
% Outputs:  R: 3x3 column-major rotation matrix
%
% author:   Troy Adebar
% date:     5/11/2015

R = [   1       0            0;
        0       cos(gamma)  -sin(gamma);
        0       sin(gamma)   cos(gamma);    ];
end

