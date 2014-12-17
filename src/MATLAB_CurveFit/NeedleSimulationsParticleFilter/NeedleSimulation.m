% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% setup params
params.dt = 1/10;                                                           %simulation timestep (s)
params.muOrientation = [0; 0; 0];                                           %orientation process noise mean
params.sigmaOrientation = diag([pi/5000, pi/5000, pi/5000]);                %orientation process noise std
params.muPos = [0; 0; 0];                                                   %position process noise mean
params.sigmaPos = zeros(3,3);%diag([.25 .25 .25]);                                      %position process noise std
params.muRho = 0;                                                           %needle radius of curvature process noise mean
params.sigmaRho = 3;                                                        %needle radius of curvature process noise std
params.simulationTime = 60;                                                 %simulate 10 seconds of insertion
params.n = 40;                                                              %only keep track of current state

params.w = 83*630*1e-3;                                                     %physical width of ultrasound frame (mm)
params.h = 83*480*1e-3;                                                     %phyiscal height of ultrasound frame (mm)
params.axialSigma = 1.5;                                                    %sigma of where the image plane is wrt to needle tip (mm)
params.frameOffsetMu = 0;                                                   %image wrt needle tip average
params.frameOffsetSigma = 83*100*1e-3;                                      %centering of ultrasound frame on needle 
params.frameOrientationMu = [0; 0; 0];                                      %frame orientation mu
params.frameOrientationSigma = diag([pi/10, pi/10, pi/10]);                 %frame orientation sigma

params.measurementOffsetMu = [0; 0];                                        %zero measurement noise offset
params.measurementOffsetSigma = diag([83*5*1e-3 83*5*1e-3]);                %5 pixels of measurement noise
params.offNeedleDopplerMu = 2;

params.writeVideo = 0;
params.videoFile = 'C:/position.avi';

% initial state of needle
xcurr.pos = [0; 0; 0];
xcurr.R = eye(3);
xcurr.rho = 60;        % 4 cm roc

flipped = false;

uc.v = 0;               %10 mm/s
uc.dc = 0;              %no duty cycle
uc.dtheta = 0;          

u{1} = uc;
u = repmat(u,params.n,1);

% simulation
figure(1);
ylim([-80 80]);
zlim([-10 100]);
xlim([-50 50]);
xlabel('x');
ylabel('y');
zlabel('z');
view(3);
axis equal;
hold on;
grid on;

writerObj = [];
if(params.writeVideo)
    writerObj = VideoWriter(params.videoFile);
    open(writerObj);
end

handles = [];
for t=0:params.dt:params.simulationTime
    uc.v = 10;
    uc.dc = 0;
    uc.dtheta = 0;
    
    if(t > 5 && ~flipped)
        uc.dtheta = pi/2;
        flipped = 1;
    else
        uc.dtheta = 0;
    end
    
    xlast = xcurr;
    xcurr = f(xcurr,uc,params);
    
    xd = [xlast.pos xcurr.pos]';
    plot3(xd(:,1), xd(:,2), xd(:,3), 'k', 'LineWidth', 2);
    hold on;
    if(~isempty(handles))
        delete(handles);
    end
    
    u(2:params.n) = u(1:params.n-1);
    u{1} = uc;
    xs = propagateNeedleBack(xcurr,u,params);
    
    handles = drawFrames(1, xs(1:4:end), 20);
    
    
    xlim([-50 50]);
    if(~isempty(writerObj))
        frame = getframe;
        writeVideo(writerObj, frame);
    end
    pause(params.dt);
end

close(writerObj);







