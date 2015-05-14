function [ R ] = Rz( gamma )
%ROTZ Creates a rotation matrix for a rotation around z.
%   [ R ] = rotz( gamma )
% Inputs:   gamma: rotation around z-axis (radians)
% Outputs:  T: 3x3 column-major rotation matrix
%
% author:   Troy Adebar
% date:     11/21/2013

R = [ cos(gamma) -sin(gamma) 0 ;
      sin(gamma)  cos(gamma) 0 ;
      0           0          1 ; ];

end

