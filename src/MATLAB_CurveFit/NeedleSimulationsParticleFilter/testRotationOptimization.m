% generate needle trajectory
clear; clc; close all;

addpath('icp');

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 0;
params.doMeasurement = 0;
params.simulationTime = 12;
params.particleInitTime = 100;

params.sigmaOrientation = diag(1/5*[1*pi/1000, 1*pi/1000, pi/1000]);            
%params.sigmaPos = zeros(3,3);
%params.sigmaRho = 0;
%params.sigmaRho = 1;
%params.sigmaVelocity = 0;

params.n = 20;
nskip = 2;

[~, xhist, u] = runSimulation(params, @(t,params)(twistyCommandFcn(t, params)));

drawPointHistory(xhist(1:nskip:end), 1, [1 1 0]);

% perturb latest orientation by small rotation
dtheta = pi/2;
khat = [1; 1; 1]; khat = khat/norm(khat);
qperturb = RotationMatrixToQuat(SO3Exp(khat*dtheta));
xcurr = xhist{1};
xcurr.q = quatmult(qperturb, xcurr.q);

xp = propagateNeedleBack(xcurr, u, params);

%drawFrames(1, {xp{1}}, 5, []); 
drawPointHistory(xp(1:nskip:end), 1, [0 0 1]);

X = cell2mat(xhist);
X = [X.pos]';
X = X-repmat(X(1,:), size(X,1), 1);
Y = cell2mat(xp);
Y = [Y.pos]';
Y = Y-repmat(Y(1,:), size(Y,1), 1);
%[~, ~, R] = procrustes(Y,X,'scaling',false, 'reflection', false);
% R.c(1,:)
% R = R.T;
R = procrustesRotation(Y,X);
%[R, ~] = icp(X', Y');
qfix = RotationMatrixToQuat(R);



%qfix = quatinverse(qperturb);
xprot = xp;
for i=1:length(xp)
    xprot{i}.pos = quatrot(qfix, xp{i}.pos-xp{1}.pos)+xp{1}.pos;
end

drawPointHistory(xprot(1:nskip:end), 1, [1 0 0]);

xfix = xp{1};
xfix.q = quatmult(qfix, xfix.q);
drawFrames(1, {xfix}, 40, []);

SO3Metric(QuatToRotationMatrix(xfix.q), QuatToRotationMatrix(xhist{1}.q))
SO3Metric(QuatToRotationMatrix(xp{1}.q), QuatToRotationMatrix(xhist{1}.q))

figure(2);
handles = [];
currx = xfix;
currx.pos = [0; 0; 0];
handles = drawFrames(2, {currx}, 1, handles);
view(3);
grid on;
xlim([-2 2]);
ylim([-2 2]);
zlim([-2 2]);
daspect([1 1 1]);
pause;
currx = xhist{1};
currx.pos = [0; 0; 0];
handles = drawFrames(2, {currx}, 1, handles);
% xlim([-5 5]);
% ylim([-5 5]);
% zlim([-5 5]);
% axis equal;
% pause;
% currx = xfix;
% currx.pos = [0; 0; 0];
% handles = drawFrames(2, {currx}, 1, handles);
