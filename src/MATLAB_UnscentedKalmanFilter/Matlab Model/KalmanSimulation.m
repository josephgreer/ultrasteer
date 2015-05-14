function [e,pf] = KalmanSimulation(x0,p,act,kal,grd)
%KALMANSIMULATION   Simulates closed-loop needle steering w/ Kalman filter
% [e] = KalmanSimulation(act,kal,grd)
% Inputs:   x0:  actual initial pose of the needle
%           p:   actual target
%           act: boolean that controls 3D plotting of simulation
%           kal: boolean that controls 3D plotting of estimation
%           grd: boolean that controls 2D plots of results
% Outputs:  e: final tip position error
%           pf: final tip position
%
% author:   Troy Adebar
% date:     5/13/2015

%% Define constants for the simulation ------------------------------------
% Steering parameters
l_max = 3;                           % insertion increments (mm)
r_min = 50;                           % minimum radius of curvature (mm)
r_max = 400;                          % minimum radius of curvature (mm)
% Size of the state and measurement vectors
n = 6;    
m = 6;
% Limit number of total time steps for each simulation
N = 200;
% Number of initial scanning steps without motion
Nscan = 10;
% Load experimentally measured noise
load('../Noise Characterization/ProcessNoise.mat');
load('../Noise Characterization/MeasurementNoise.mat');
% Initial needle pose
x(:,:,1) = x0;
% Simulate initial measurements
v = diag(V(1:6,randi(length(V),6,1)));
% v = Re^0.5*randn(m,1) + mu_ve;
z(:,:,1) = g(x(:,:,1),v);
% Kalman filter process noise parameters
mu_w = zeros(n,1);                    
Q = diag([0.2,0.2,0.2,0.001,0.001,0.001]);
% Kalman filter observation noise parameters
mu_v = zeros(m,1);                    
R = diag([0.1,0.1,0.1,1,1,1]);   
% Define prior distribution for the estimate
x_hat0 = [eye(3) [0 50 -40]'; 0 0 0 1];    % [R p]
P_0 = diag([1 1 1 0.01 0.01 0.01].^2);
% Run the first iteration of the Kalman filter
[x_hat(:,:,1),P(:,:,1),K(:,:,1)] = ukf( x_hat0,P_0,Q,R,[0 10 0]',z(:,:,1));
% Define tolerance for assuming the tip has reached the target
epsilon = 0.5;

%% Run simulation loop ----------------------------------------------------
for t = 1:N
    % Simulate process and measurement noise
    w = diag(W(1:6,randi(length(W),6,1)));
    v = diag(V(1:6,randi(length(V),6,1)));
     
    % Define control inputs for the current time based on the estimate
    [u(:,:,t)] = steeringPlanner(x_hat(:,:,t),p);
    % Limit the insertion increment
    if t <= Nscan
        u(3,1,t) = 0;
        w = zeros(6,1);
    else
        u(3,1,t) = min(u(3,1,t),l_max);
    end
    
    % Limit the radius to a known minimum and maximum
    u(2,1,t) = max(r_min,u(2,1,t));
    u(2,1,t) = min(r_max,u(2,1,t));
  
    % Propogate the actual state forward (simulation)
    x(:,:,t+1) = f(x(:,:,t),u(:,:,t),w);
    % Simulate measurements (simulation)
    z(:,:,t+1) = g(x(:,:,t+1),v);
    
    % Apply an unscented Kalman filter and update state estimate
    [x_hat(:,:,t+1),P(:,:,t+1),K(:,:,t+1)] = ukf( x_hat(:,:,t),...
        P(:,:,t),Q,R,u(:,:,t),z(:,:,t+1) );
      
    % Check whether the target has been reached
    if( checkCompletion(x_hat(:,:,t+1),p,epsilon) ) % If it has, quit sim
        break;
    end
end
%% Display final tip error in command window
e = norm(x(1:3,4,end)-p);
pf = x(1:3,:,end);
%% Plots results
kalmanPlots(act,kal,grd,p,x,x_hat,z,u,K)
end
%eof