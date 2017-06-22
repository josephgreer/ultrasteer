clear; clc; close all;

addpath('../../../../../../../MATLAB_CurveFit/NeedleSimulationsParticleFilter/');
addpath('../');

n_actuators = 3;
leq = [80; 80; 80; 100];
ks = [0; 0; 0; 1; 10];
d = 5;
ll = leq;
psis = actuatorThetas();

system = @(l)(SpringSystemEquations(l, ks, leq, d));
[ls, fval] = fsolve(system, leq(1:n_actuators));
ls = real(ls);
% xs = ls;

systemFull = @(x)(FullSpringSystemEquations(x, ks, leq, d, psis,1));
[xs, fval] = fsolve(systemFull, leq(1:n_actuators));
xs = real(xs);
ls = xs(1:3);

controlParams.targetPosition = [0;0;200];
controlParams.targetGoal = [0;0];
controlParams.targetSize = 25;
controlParams.focalLength = 182/5; % expressed in pixels
controlParams.kp = 0.5;
controlParams.q = zeros(3,1);
log.errors = [];
log.us = [];

handles = [];
h = figure;
set(h,'Position',[0 0 900 400]);
subplot(1,2,1);
handles = drawVine(xs,d,handles);
handles = drawImage(xs,d,controlParams,handles);
daspect([1 1 1]);
view([15 45]);
xlim([-80 80]);
ylim([-80 80]);
zlim([0 250]);
xlabel('x');
ylabel('y');
zlabel('z');
grid on;
htitle = title(sprintf('k = [%f %f %f] fval=[%f %f %f]', ks(1), ks(2), ks(3),...
    fval(1), fval(2),fval(3)));

handles.targetPos = scatter3(controlParams.targetPosition(1),controlParams.targetPosition(2),controlParams.targetPosition(3),200,'filled');
hold on;

% calibrate Jacobian
J = eye(2,3);
kk = diag([0.05 0.05 0.05 0]);
kk = kk(1:3,:);

for i=1:4
    ks(1:3) = kk(:,i);
    systemFull = @(x)(FullSpringSystemEquations(x, ks, leq, d, psis,1));
    [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
    xs = real(xs);
    ls = xs(1:3);
    handles = drawVine(xs,d,handles);
    [handles,pos,~] = drawImage(xs,d,controlParams,handles);
    if(i < 4)
        J(:,i) = pos(1:2)/norm(kk(:,i));
    end
%     pause(0.5);
end


% this is the point we're taking approximate jacobian around
k0 = [0.05; 0.05; 0.00];
ks(1:3) = k0;
systemFull = @(x)(FullSpringSystemEquations(x, ks, leq, d, psis,1));
[xs, fval] = fsolve(systemFull, leq(1:n_actuators));
xs = real(xs);
ls = xs(1:3);
[theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
arcSpace0 = [phi; theta; l; ls(1); ls(2); ls(3)];
tip0 = kinematicParametersToTipPoint(l,phi,theta,eye(3),zeros(3,1));
R0 = KinematicParametersToTipFrame(phi,theta,l);
im0 = KinematicParametersToImagePoint(phi,theta,l,controlParams);

% approximate jacobian
JJarc = zeros(6,4);
JJcart = zeros(6,4);
JJvis = zeros(2,4);

alphas = [1.0001; 1; 1; 1];
kk = diag([0 0.0001 0.0001 0.0001]);
kk = kk(2:end,:);
kk = repmat(k0,1,4)+kk;
for i=1:4
    alpha = alphas(i);
    ks(1:3) = kk(:,i);
    kcurr = ks/alpha;
    leqs = leq*alpha;
    
    
    systemFull = @(x)(FullSpringSystemEquations(x, ks, leqs, d, psis,1));
    [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
    xs = real(xs);
    ls = xs(1:3);
    [theta,l,phi] = lengthsToLThetaPhiNonUniform(ls,d,psis);
    tip = kinematicParametersToTipPoint(l,phi,theta,eye(3),zeros(3,1));
    R = KinematicParametersToTipFrame(phi,theta,l);
    dR = R*R0.';
    dOmega = SO3HatInverse(SO3Log(dR));
    im = KinematicParametersToImagePoint(phi,theta,l,controlParams);
    
    arcSpace = [phi; theta; l; ls(1); ls(2); ls(3)];
    JJarc(:,i) = (arcSpace-arcSpace0);
    JJcart(:,i) = [tip-tip0;dOmega];
    JJvis(:,i) = im-im0;
    if i==1
        JJarc(:,i) = JJarc(:,i)/abs(alpha-1);
        JJcart(:,i) = JJcart(:,i)/abs(alpha-1);
        JJvis(:,i) = JJvis(:,i)/abs(alpha-1);
    end
    if(i > 1)
        JJarc(:,i) = JJarc(:,i)/norm(kk(:,i)-k0);
        JJcart(:,i) = JJcart(:,i)/norm(kk(:,i)-k0);
        JJvis(:,i) = JJvis(:,i)/norm(kk(:,i)-k0);
    end
    
end

ks(1:3) = k0;
systemFull = @(x)(FullSpringSystemEquations(x, ks, leqs, d, psis,1));
[xs, fval] = fsolve(systemFull, leq(1:n_actuators));
xs = real(xs);
ls = xs(1:3);
JJArcAnalytical = JacobianArcSpace(ls,ks,1,leqs,d,psis);
JJCartAnalytical = JacobianCartesian(ls,ks,1,leqs,d,psis);
JJVisAnalytical = JacobianVisual(controlParams,ls,ks,alpha,leqs,d,psis);

for i=1:4
    display(sprintf('i = %f', atan2(JJvis(2,i),JJvis(1,i))));
end


controlParams.targetPosition = [100;100;110]; 
xlim([min(-80,controlParams.targetPosition(1)) max(80,controlParams.targetPosition(1))]);
ylim([min(-80,controlParams.targetPosition(2)) max(80,controlParams.targetPosition(2))]);   
systemFull = @(x)(FullSpringSystemEquations(x, ks, leq, d, psis,1));
[xs, fval] = fsolve(systemFull, leq(1:n_actuators));
xs = real(xs);
ls = xs(1:3);

assert(norm(xs(1:3)-ls)<1e-4);

set(handles.targetPos,'XData',controlParams.targetPosition(1),'YData',controlParams.targetPosition(2),'ZData',controlParams.targetPosition(3));
handles = drawVine(xs,d,handles);
[handles,pos,~] = drawImage(xs,d,controlParams,handles);

randDisturbance = deg2rad(45);
randDisturbance = Rz(randDisturbance);
randDisturbance = randDisturbance(1:2,1:2);
% J = randDisturbance*J;
% J(:,1) = 1*J(:,1);
% J(:,2) = 1*J(:,2);
% J(:,3) = 1*J(:,3);


while(true)
    q = ks(1:3);
    errorTrue = controlParams.targetGoal-pos(1:2);
    error = errorTrue+randn(2,1)*0;
    dx = controlParams.kp*error;
    dq = BVLS(J,dx,-q,1-q);
%     dq = pinv(J)*dx;
    q = q+dq;
    q = min(max(q,0),1);
    display(ks);
    ks(1:3) = q;
    log.errors = vertcat(log.errors,errorTrue);
    log.us = vertcat(log.us,q);
    
    systemFull = @(x)(FullSpringSystemEquations(x, ks, leq, d, psis,1));
    [xs, fval] = fsolve(systemFull, leq(1:n_actuators));
    xs = real(xs);
    ls = xs(1:3);
    
    assert(norm(xs(1:3)-ls)<1e-4);
    
    handles = drawVine(xs,d,handles);
    [handles,pos,~] = drawImage(xs,d,controlParams,handles);
    set(htitle, 'String', sprintf('k = [%f %f %f], error=[%f %f]', ks(1), ks(2), ks(3),...
    errorTrue(1),errorTrue(2)));
    pause(0.05);
end
