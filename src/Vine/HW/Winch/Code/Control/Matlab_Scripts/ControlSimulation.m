clear; clc; close all;

addpath('../cvxgen/');

rng(35);

nActuators = 3;                                                              % number of turning actuators
deltaThetas = mod(2*pi/3*[0:2],2*pi).';                                      % spacing between turning actuators
theta0 = 0;                                                                  % angle of first actuator
nPoints = 100;                                                               % number of time-steps for simulation
saveQs = false;                                                              % whether or not to save q-vals for test
loadQs = true;                                                              % whether or not to load q-vals for test
strengthMean = 6;                                                          % mean strength
strengthSigma = 0.5;                                                           % strength standard deviation
measurementSigma = strengthMean*0.05;                                        % measurement noise variance
thetaSigma = 1e-1;                        
eps = 1e-6;
doPause = false;
doControlOutput = false;

% initialize robot
xact = [strengthMean*ones(nActuators,1); theta0];
pos = [0; 0];
q = ones(nActuators, 1);
deltaStrength = strengthMean;
R = diag([strengthSigma^2*ones(nActuators,1); thetaSigma^2]);
Q = eye(2)*measurementSigma^2;
Sigma = 1e-1*eye(nActuators+1);                                                                % initial sigma
xest = xact;%[50; 300; 200; 0.5];

display(sprintf('f64 strengths[3] = {%f, %f, %f};', xact(1), xact(2), xact(3)));
display(sprintf('f64 thetas[3] = {%f, %f, %f};', xact(4)+deltaThetas(1), xact(4)+deltaThetas(2), xact(4)+deltaThetas(3)));
display(sprintf('f64 E0[4] = {%f, %f, %f, %f};', Sigma(1,1), Sigma(2,2), Sigma(3,3), Sigma(4,4)));
display(sprintf('f64 Q[2] = {%f, %f};', Q(1,1), Q(2,2)));
display(sprintf('f64 R[4] = {%f, %f, %f, %f};', R(1,1),R(2,2),R(3,3),R(4,4)));


% initialize display
handles = [];
figure;
hold on;
daspect([1 1 1]);
xlim([-800 800]);
ylim([-800 800]);

% initialize drawing of the workspace
drawWorkspace(StateToTheta0(xact)+deltaThetas, StateToStrengths(xact,nActuators));

% initialize logging
qsave = [];
qload = [];
if(loadQs)
    qload = load('saveqs.mat');
    qload = qload.qsave;
end

xests = zeros(4,nPoints);
xacts = zeros(4,nPoints);
dqs = zeros(nActuators,nPoints);

% for each time-step...
for i=1:nPoints
%     display(i);
    handles = drawRobot(handles, pos, StateToTheta0(xact)+deltaThetas, q);
    
    % create a random vector delta_x to move the robot
    theta_offset = unifrnd(0, 2*pi);
    delta_x_desired = unifrnd(0,deltaStrength)*[cos(theta_offset); sin(theta_offset)];
    
    % draw the random vector
    if(~exist('quiverHandle'))
        quiverHandle = quiver(pos(1), pos(2), delta_x_desired(1), delta_x_desired(2));
    end
    quiverHandle.XData = pos(1); quiverHandle.YData = pos(2);
    quiverHandle.UData = delta_x_desired(1); quiverHandle.VData = delta_x_desired(2);
    
    if(doPause)
        pause(0.5);
    end
    
    % Form estimated and actual jacobian
    [Jest,~] = FormJacobian(xest,deltaThetas,nActuators);
    [J,~] = FormJacobian(xact,deltaThetas,nActuators);
    
    % find the dq to best achieve delta_x_desired
    dq = BVLS(J,delta_x_desired,-q,1-q);
    q = q+dq; 
    if(~all(0 <= q & q <= 1))
        yep = 0;
    end
    % what is the actuatl delta_x we achieved?
    delta_x_actual = J*dq;
    delta_x_measured = delta_x_actual+mvnrnd([0;0],Q).';
    
    % update the robot
    pos = pos+delta_x_actual;
    
%     pause(1);
    if(saveQs)
        qsave = [qsave q];
    end
    if(loadQs)
        q = qload(:,i);
    end
    
    % estimate jacobian
    [xest, Sigma] = JacobianExtendedKalmanFilter(delta_x_measured, xest, dq, Sigma, R, Q, nActuators, deltaThetas);
    
    %display the results
    if(doControlOutput)
        display(sprintf('{%f, %f, %f, %f, %f, %.8f},', delta_x_desired(1), delta_x_desired(2),...
            q(1), q(2), q(3), norm(delta_x_desired-delta_x_actual)));
    else
        display(sprintf('{%f, %f, %f, %f, %f, %f, %f, %f, %f},', xest(1), xest(2),...
            xest(3), xest(4), delta_x_measured(1), delta_x_measured(2), dq(1), dq(2), dq(3)));
    end
    
    xacts(:,i) = xact;
    xests(:,i) = xest;
    dqs(:,i) = dq;
    
    % perturb jacobian
    xact = xact+mvnrnd(zeros(4,1),R).';
end

if(saveQs)
    save('saveqs.mat', 'qsave');
end

figure;
subplot(2,2,1);
hold on;
plot(1:nPoints,xests(1,:),'b--');
plot(1:nPoints,xacts(1,:),'r');
title('strength 1');
subplot(2,2,2);
hold on;
plot(1:nPoints,xests(2,:),'b--');
plot(1:nPoints,xacts(2,:),'r');
title('strength 2');
subplot(2,2,3);
hold on;
plot(1:nPoints,xests(3,:),'b--');
plot(1:nPoints,xacts(3,:),'r');
title('strength 3');
subplot(2,2,4);
hold on;
plot(1:nPoints,xests(4,:),'b--');
plot(1:nPoints,xacts(4,:),'r');
title('theta');

figure;
subplot(2,2,1);
plot(1:nPoints,dqs(1,:),'r');
subplot(2,2,2);
plot(1:nPoints,dqs(2,:),'r');
subplot(2,2,3);
plot(1:nPoints,dqs(3,:),'r');
