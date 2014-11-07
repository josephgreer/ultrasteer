function x_2 = f(x_1,u_1,w_1)
% f Non-linear state equations for duty-cycled needle steering
%    x_2 = f(x_1,u_1,w_1) returns updated state vector that results from
%    system dynamics, input, and gaussian noise vector:
%           x_k+1 = f(x_k,u_k) + w_k
% Inputs:   x_1: 6x1 state vector: [p (3x1 pos), r (3x1 rot)]' 
%           u_1: 3x1 input vector: [d_th, r, l]'
%           w_1: 6x1 noise vector: [w_p (3x1 pos), w_q (3x1 rot)]'
% Output:   x_2: updated state vector

%% Initialize variables
x_2 = zeros(6,1);
% Isolate state position vector and rotation vector
p = x_1(1:3);
r = x_1(4:6);
% Change rotation vector to quaternion representation
q = rot2quat(r);
% Isolate specific input variables
d_th = u_1(1);
r = u_1(2);
l = u_1(3);

%% Update position
% Calculate next needle tip position p in current needle tip frame...
% Use a rotation of d_th around z-axis, followed by circ traj in y-z plane
d_p = [cos(d_th) -sin(d_th) 0;
       sin(d_th)  cos(d_th) 0;
       0          0         1 ]*[0 r*(1-cos(l/r)) r*(sin(l/r))]';
% Transform delta vector into world frame using quaternion rotation
d_p = ( quatrotate(quatinv(q),d_p') )';
% Add vector to original position
x_2(1:3) = p + d_p + w_1(1:3);

%% Update orientation quaternion
% Calculate noise quaternion from random rotation vector
[q_w]=rot2quat(w_1(4:6));
% Form quaternions for x and z-axis rotations
q_a = [cos(-l/r/2) sin(-l/r/2).*[1 0 0]]; 
q_b = [cos(d_th/2) sin(d_th/2).*[0 0 1]];
q_2 = quatmultiply(q, quatmultiply(q_w, quatmultiply( q_b,q_a ) ) );
% Change quaternion back to rotation vector
x_2(4:6) = quat2rot(q_2);

%% eof


