function z = g(x,v)
% f measurement equations for duty-cycled needle steering
%    z = g(x) returns measurement vector given the current state and. 
%    The measurement consists of the needle tip position
%    in R^3, and the needle tangent vector in R^3 (i.e., the z-axis of the
%    needle's tip frame.
% Inputs:   x_1: 4x4 state matrix: [R (3x3 rotation), p (3x1 pos); 0 0 0 1]
%             v: 6x1 noise vector 
% Output:   z: 4x4 state matrix: [R (3x3 rotation), p (3x1 pos); 0 0 0 1]

% Initialize variables
z = x;

% Calculate noise rotation matrix and pos vec from noise vec
Rw = vec2mat(v(4:6));
pw = v(1:3);

% Add position error
z(1:3,4) = z(1:3,4) + pw;

% Add orientation error
z(1:3,1:3) = z(1:3,1:3)*Rw;

%% eof