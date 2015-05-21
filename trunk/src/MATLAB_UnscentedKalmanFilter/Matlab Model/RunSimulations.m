% -------------------------------------------------------------------------
% author:         Troy Adebar
% title:          RunSimulations
% date:           10/8/2013
% brief:          Simulates closed loop needle steering using ultrasound
%                 image feedback (simulated). Calls ClosedLoopSimulation
%                 which actually runs the simulation
% -------------------------------------------------------------------------
%% Initialize
 clc;
 addpath('Utility Functions');

%% Define constants
Nsim = 1;
rho_min = 50;                          % minimum radius of curvature (mm)
rho_max = 500;                         % minimum radius of curvature (mm)
x0 = [eye(3) [0 50 -40]'; 0 0 0 1];    % [R p]
E = diag([0.01 0.01 0.01]);            % Variance of initial orientation

e_normal = zeros(Nsim,1);
e_UKF = zeros(Nsim,1);
ts = zeros(3,Nsim);
pf_normal = zeros(3,Nsim);
pf_UKF = zeros(3,Nsim);

i = 1;
%% Run the simulations with Kalman filtering
% Generate a target
t = [randi([-30 -25]) randi([35 65]) randi([35 45])]';
ts(:,i) = t;

% Randomly adjust the initial orientation
x0(1:3,1:3) = vec2mat(E^0.5*randn(3,1));

t_e = [];
% With UKF
[error,pf] = KalmanSimulation(x0,t,true,true,true)


