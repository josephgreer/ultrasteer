function [ rho,c,pts,v_e ] = findCircle( a,b,v_i,N )
%FINDCIRCLE Summary of this function goes here
% [ rho,c,pts ] = findCircle( a,b,v )
% Inputs:   a:          point (3x1) on the circle
%           b:          point (3x1) on the circle
%           v_i:        tangent vector (3x1) at a
%           N:          number of requested output points on arc [a,b]
% Outputs:  rho:        radius of the circle
%           c:          center point (3x1) of the circle
%           pts:        output pts (3xN) on arc [a,b]
%           v_e:        tangent vector (3x1) at b
% 
% author:   Troy Adebar
% date:     2/24/2015
% -------------------------------------------------------------------------
%% Define unit vectors
% define "y-axis" unit vector in the direction of v
u_2 = v_i./norm(v_i);
% define "x-axis" unit vector using projection of b-a
v_b = b-a;
v_bp = v_b-dot(v_b,u_2).*u_2;
u_1 = v_bp./norm(v_bp);
% define "z-axis" unit vector
u_3 = cross(u_1,u_2);


%% Project vector onto unit vectors
x = dot(u_1,v_b);
y = dot(u_2,v_b);


%% Solve for radius and center
rho = ( x^2 + y^2 )/2/abs(x);
c = a + rho.*u_1;


%% Find arc angles at a and b
theta_a = pi;                           % angle at a; by construction
R = inv([u_1 u_2 u_3]);                 % define basis transform
h = R*(b-c);                            % convert vector to u_1,u_2 basis
theta_b = atan2(h(2)/rho,h(1)/rho);     % find angle using ratio


%% Find N points between a and b 
theta = linspace(theta_b,theta_a,N);
pts = [];
for th = theta
    pts = [pts c + rho*u_1*cos(th) + rho*u_2*sin(th)];
end

%% Find tangent vector at end point
r = [ u_3' theta_b - theta_a ];
R = vrrotvec2mat(r);
v_e = R*v_i;

end

