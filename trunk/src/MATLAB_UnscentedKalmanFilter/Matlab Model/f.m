function x_2 = f(x_1,u_1,w_1)
% f Non-linear state equations for duty-cycled needle steering
%    x_2 = f(x_1,u_1,w_1) returns updated state vector that results from
%    system dynamics, input, and gaussian noise vector:
%           x_k+1 = f(x_k,u_k) + w_k
% Inputs:   x_1: 4x4 state matrix: [R (3x3 rotation), p (3x1 pos); 0 0 0 1] 
%           u_1: 3x1 input vector: [d_th, r, l]'
%           w_1: 6x1 noise vector: [w_p (3x1 pos), w_q (3x1 rot)]'
% Output:   x_2: updated state vector

%% Initialize variables
x_2 = zeros(4,4);
x_2(4,4) = 1;
% Isolate state position vector and rotation vector
p_1 = x_1(1:3,4);
R_1 = x_1(1:3,1:3);
% Isolate specific input variables
d_th = u_1(1);
rho = u_1(2);
l = u_1(3);

%% Update position
% Calculate next needle tip position p in current needle tip frame...
% Use circ traj in y-z plane
d_p = [0 rho*(1-cos(l/rho)) rho*(sin(l/rho))]';
% Add vector to original position
x_2(1:3,4) = p_1 + R_1*Rz(d_th)*d_p + w_1(1:3);

%% Update orientation quaternion
% Calculate noise rotation matrix from random rotation vector
Rw = vec2mat(w_1(4:6));
% Update rotation matrix with noise, z-axis and x-axis rotation
x_2(1:3,1:3) = R_1*Rw*Rz(d_th)*Rx(-l/rho);

%% eof


