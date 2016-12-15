clear; clc; close all;

rng(15);

nActuators = 3;                                                              % number of turning actuators
deltaThetas = mod(2*pi/3*[0:2],2*pi).';                                      % spacing between turning actuators
theta0 = 0;                                                                  % angle of first actuator
nPoints = 50;                                                               % number of time-steps for simulation
saveQs = false;                                                              % whether or not to save q-vals for test
loadQs = false;                                                              % whether or not to load q-vals for test
strengthMean = 3;                                                            % strenght mean
measurementSigma = 1e-2;                                                     % measurement noise variance                   
eps = 1e-6;
doPause = false;
doControlOutput = false;

% initialize robot
xact = FormJacobian([strengthMean*ones(nActuators,1); theta0], deltaThetas, nActuators);
xact = xact(:);
pos = [0; 0];
q = ones(nActuators, 1);
deltaStrength = strengthMean;
R = eye(length(xact))*strengthMean*1e-3;
Q = eye(2)*measurementSigma^2;
Sigma = 1e-1*eye(length(xact));                                                                % initial sigma
xest = xact;%[50; 300; 200; 0.5];

J0 = reshape(xact,2,3);
display(sprintf('f64 J0[2][3] = {{%f, %f, %f}, {%f, %f, %f}};', J0(1,1), J0(1,2), J0(1,3), J0(2,1), J0(2,2), J0(2,3)));
display(sprintf('f64 E0[6] = {%f, %f, %f, %f, %f, %f};', Sigma(1,1), Sigma(2,2), Sigma(3,3), Sigma(4,4), Sigma(5,5), Sigma(6,6)));
display(sprintf('f64 Q[2] = {%f, %f};', Q(1,1), Q(2,2)));
display(sprintf('f64 R[6] = {%f, %f, %f, %f, %f, %f};', R(1,1),R(2,2),R(3,3),R(4,4),R(5,5),R(6,6)));


% initialize display
handles = [];
figure;
hold on;
daspect([1 1 1]);
xlim([-800 800]);
ylim([-800 800]);

% initialize drawing of the workspace
drawWorkspace(theta0+deltaThetas, strengthMean*ones(nActuators,1));

% initialize logging
qsave = [];
qload = [];
if(loadQs)
    qload = load('saveqs.mat');
    qload = qload.qsave;
end

xests = zeros(2*nActuators,nPoints);
xacts = zeros(2*nActuators,nPoints);
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
    
    Jest = reshape(xest,2,nActuators);
    J = reshape(xact,2,nActuators);
    
    % find the dq to best achieve delta_x_desired
    dq = BVLS(Jest,delta_x_desired,-q,1-q);
    q = q+dq; 
    if(~all(0 <= q & q <= 1))
        yep = 0;
    end
    % what is the actuatl delta_x we achieved?
    delta_x_actual = J*dq;
    delta_x_measured = delta_x_actual+mvnrnd([0;0],Q).';
    
    % update the robot
    pos = pos+delta_x_actual;
    
    %pause(1);
    if(saveQs)
        qsave = [qsave q];
    end
    if(loadQs)
        q = qload(:,i);
    end
    
    % estimate jacobian
    [xest, Sigma] = JacobianKalmanFilter(delta_x_measured, xest, dq, Sigma, R, Q, nActuators);
    
    %display the results
    if(doControlOutput)
        display(sprintf('{%f, %f, %f, %f, %f, %.8f},', delta_x_desired(1), delta_x_desired(2),...
            q(1), q(2), q(3), norm(delta_x_desired-delta_x_actual)));
    else
        display(sprintf('{%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f},', xest(1), xest(2),...
            xest(3), xest(4), xest(5), xest(6), delta_x_measured(1), delta_x_measured(2), dq(1), dq(2), dq(3)));
    end
    
    xacts(:,i) = xact;
    xests(:,i) = xest;
    dqs(:,i) = dq;
    
    % perturb jacobian
    xact = xact+mvnrnd(zeros(6,1),R).';
end

if(saveQs)
    save('saveqs.mat', 'qsave');
end

figure;
errors = sqrt(sum((xests-xacts).^2,1));
plot(1:nPoints,errors);
title('errors');

figure;
subplot(2,3,1);
hold on;
plot(1:nPoints,xests(1,:),'b--');
plot(1:nPoints,xacts(1,:),'r');
title('j11');
subplot(2,3,4);
hold on;
plot(1:nPoints,xests(2,:),'b--');
plot(1:nPoints,xacts(2,:),'r');
title('j21');
subplot(2,3,2);
hold on;
plot(1:nPoints,xests(3,:),'b--');
plot(1:nPoints,xacts(3,:),'r');
title('j12');
subplot(2,3,5);
hold on;
plot(1:nPoints,xests(4,:),'b--');
plot(1:nPoints,xacts(4,:),'r');
title('j22');
subplot(2,3,3);
hold on;
plot(1:nPoints,xests(5,:),'b--');
plot(1:nPoints,xacts(5,:),'r');
title('j13');
subplot(2,3,6);
hold on;
plot(1:nPoints,xests(6,:),'b--');
plot(1:nPoints,xacts(6,:),'r');
title('j23');

figure;
subplot(2,2,1);
plot(1:nPoints,dqs(1,:),'r');
subplot(2,2,2);
plot(1:nPoints,dqs(2,:),'r');
subplot(2,2,3);
plot(1:nPoints,dqs(3,:),'r');
