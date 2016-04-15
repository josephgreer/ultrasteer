% generate needle trajectory
clear; clc; close all;

addpath('icp');

rng(3);

% init params
params = initParamsForSimulation();

% custom parameters
params.doParticleFilter = 0;
params.doMeasurement = 0;
params.simulationTime = 10;
params.particleInitTime = 100;

params.sigmaOrientation = diag(0/5*[1*pi/1000, 1*pi/1000, pi/1000]);            
params.sigmaPos = zeros(3,3);
params.sigmaRho = 0;
%params.sigmaRho = 1;
params.sigmaVelocity = 0;

params.writeVideo = 1;
params.videoFile = 'C:/Joey/test.mp4';

params.minLength = 40;

params.n = 100;
nskip = 10;
qs = {};

[~, xhist, u, writerObj] = runSimulation(params, @(t,params)(commandFcn(t, params)),[],[],[]);
pause;

xhists = {xhist};

uhist = u;%{u{end:-1:1}};
xinit = xhist{1};
qs = {xinit.q}
initQ = xinit.q;
xinit.q = quatmult(xinit.q, AxisAngleToQuat(pi*[0; 1; 0]));
[~, ~, ~, writerObj] = runSimulation(params, @(t, params)(historyCommandFcn(t, params, uhist)), [], xinit,writerObj)

close(1);
figure(1);
pos = get(1, 'position');
set(1, 'position', [pos(1)-150 pos(2)-150 1*pos(3) 1.5*pos(4)]);
ylim([-30 150]);
zlim([-30 150]);
xlim([-75 75]);
xlabel('x');
ylabel('y');
zlabel('z');
view(45,45);
daspect([1 1 1]);
hold on;
grid on;


npauseFrames = 30*2;
pointHandles = [];
pointsOriginal = drawPointHistory(xhist(1:nskip:end), 1, 0.5*[1 1 0], []);
if(params.writeVideo)
    for j=1:npauseFrames
        frame = getframe(1);
        writeVideo(writerObj, frame);
    end
else
    pause;
end



nPerturbs = 5;
tippy = [];
for i=1:nPerturbs
    uhist = u;%{u{end:-1:1}};
    perturbVector = unifrnd(-1, 1, 3, 1);
    perturbVector = 1.5*perturbVector/norm(perturbVector);
    xinit.q = initQ;
    xinit.q = quatmult(xinit.q, AxisAngleToQuat(perturbVector));
    qs = vertcat(qs, {xinit.q});
    tippy = drawFrames(1, {xinit}, 20, params, tippy);
    if(params.writeVideo)
        for j=1:npauseFrames
            frame = getframe(1);
            writeVideo(writerObj, frame);
        end
    else
        pause;
    end
    xinit.q = quatmult(xinit.q, AxisAngleToQuat(pi*[0; 1; 0]));
    [~, xhist, ~, writerObj] = runSimulation(params, @(t, params)(historyCommandFcn(t, params, uhist)), [], xinit, writerObj);
    xhists = vertcat(xhists, {xhist});
    pointHandles = drawPointHistory(xhist(1:nskip:end), 1, [1 0 1], pointHandles);
    if(params.writeVideo)
        for j=1:npauseFrames
            frame = getframe(1);
            writeVideo(writerObj, frame);
        end
    else
        pause;
    end
end

xinit.q = initQ;
tippy = drawFrames(1, {xinit}, 20, params, tippy);
if(params.writeVideo)
    for j=1:npauseFrames
        frame = getframe(1);
        writeVideo(writerObj, frame);
    end
else
    pause;
end
xinit.q = quatmult(xinit.q, AxisAngleToQuat(pi*[0; 1; 0]));
[~, xhist, ~, writerObj] = runSimulation(params, @(t, params)(historyCommandFcn(t, params, uhist)), [], xinit, writerObj);
delete(pointsOriginal);
drawPointHistory(xhist(1:nskip:end), 1, [1 0 1], pointHandles);
if(params.writeVideo)
    for j=1:npauseFrames
        frame = getframe(1);
        writeVideo(writerObj, frame);
    end
else
    pause;
end
close(writerObj);
pause;

for i=2:length(xhists)
    xinit.q = initQ;
    tippy = drawFrames(1, {xinit}, 20, params, tippy);
    drawPointHistory(xhists{1}(1:nskip:end), 1, [1 1 0], pointHandles);
    if(params.writeVideo)
        for j=1:npauseFrames
            frame = getframe(1);
            writeVideo(writerObj, frame);
        end
    else
        pause;
    end
    xinit.q = qs{i};
    tippy = drawFrames(1, {xinit}, 20, params, tippy);
    drawPointHistory(xhists{i}(1:nskip:end), 1, [0 1 1], pointHandles);
    if(params.writeVideo)
        for j=1:npauseFrames
            frame = getframe(1);
            writeVideo(writerObj, frame);
        end
    else
        pause;
    end
end

close(writerObj);
pause;

% perturb latest orientation by small rotation
dtheta = pi/2;
khat = [1; 1; 1]; khat = khat/norm(khat);
qperturb = RotationMatrixToQuat(SO3Exp(khat*dtheta));
xcurr = xhist{1};
xcurr.q = quatmult(qperturb, xcurr.q);

xp = propagateNeedleBack(xcurr, u, params);

%drawFrames(1, {xp{1}}, 5, params, []); 
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
drawFrames(1, {xfix}, 40, params, []);

SO3Metric(QuatToRotationMatrix(xfix.q), QuatToRotationMatrix(xhist{1}.q))
SO3Metric(QuatToRotationMatrix(xp{1}.q), QuatToRotationMatrix(xhist{1}.q))

figure(2);
handles = [];
currx = xfix;
currx.pos = [0; 0; 0];
handles = drawFrames(2, {currx}, 1, params, handles);
view(3);
grid on;
xlim([-2 2]);
ylim([-2 2]);
zlim([-2 2]);
daspect([1 1 1]);
pause;
currx = xhist{1};
currx.pos = [0; 0; 0];
handles = drawFrames(2, {currx}, 1, params, handles);
% xlim([-5 5]);
% ylim([-5 5]);
% zlim([-5 5]);
% axis equal;
% pause;
% currx = xfix;
% currx.pos = [0; 0; 0];
% handles = drawFrames(2, {currx}, 1, parmas, handles);
