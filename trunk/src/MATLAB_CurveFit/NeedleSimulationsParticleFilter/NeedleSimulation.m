% this script is for simulating a needle insertion with random control
% inputs
clear; clc; close all;

% setup params

%simulation parameters
params.dt = 1/10;                                                           %simulation timestep (s)

% process noise parameters
params.muOrientation = [0; 0; 0];                                           %orientation process noise mean
params.sigmaOrientation = diag(1/1*[pi/1000, pi/1000, pi/1000]);                %orientation process noise std
params.muPos = [0; 0; 0];                                                   %position process noise mean
params.sigmaPos = zeros(3,3);%diag([.25 .25 .25]);                          %position process noise std
params.muRho = 0;                                                           %needle radius of curvature process noise mean
params.sigmaRho = 3;                                                        %needle radius of curvature process noise std
params.simulationTime = 15;                                                 %simulate 10 seconds of insertion

% ultrasound dimensions/parameters
params.usw = 83*630*1e-3;                                                   %physical width of ultrasound frame (mm)
params.ush = 83*480*1e-3;                                                   %phyiscal height of ultrasound frame (mm)
params.axialMu = 0;                                                         %(>0) ahead of needle tip, (<0) behind needle tip 0 is on
params.axialSigma = 30;                                                     %sigma of where the image plane is wrt to needle tip (mm)
params.frameOrientationMu = [0; 0; 0];                                      %frame orientation mu
params.frameOrientationSigma = diag([pi/1000, pi/1000, pi/1000]);           %frame orientation sigma

% measurement noise parameters
params.measurementOffsetMu = [0; 0];                                        %zero measurement noise offset
params.measurementOffsetSigma = diag([83*20*1e-3 83*20*1e-3]);              %5 pixels of measurement noise
params.offNeedleDopplerMode = 2;                                            %Doppler strength of measurement off needle distributed according to lognormal dist
params.offNeedleDopplerSigma = 0.2;                                         %Doppler strength of measurement off needle distributed according to lognormal dist
params.onNeedleDopplerMode = 10;                                            %Doppler strength of measurement on needle distributed according to lognormal dist
params.onNeedleDopplerSigma = 0.2;                                          %Doppler strength of measurement on needle distributed according to lognormal dist

% particle filter parameters
params.n = 40;                                                              %only keep track of current state
params.np = 100;                                                            %number of particles
params.initOrientationSigma = diag(5*[pi/1000, pi/1000, pi/100]);             %orientation sigma for initial distribution of particles
params.initOrientationMu = [0;0;0];                                   %orientation mu for initial distribution of particles
params.initRhoSigma = 3;                                                    %rho sigma for initial distribution of particles
params.initRhoMu = 0;                                                       %rho mu for initial distribution of particles
params.initPosSigma = diag([5 5 5]);                                        %pos sigma for initial distribution of particles
params.initPosMu = [0; 0; 0];                                               %pos mu for initial distribution of particles
params.particlesInit = 0;                                                    %are particles initialized?

% drawing parameters
params.drawMeasurement = 1; 
params.drawUSFrame = 0;
params.drawTipFrame = 0;
params.drawPastTipFrames = 0;
params.drawParticlePos = 0;
params.drawParticleOrientation = 0;
params.drawParticlesNs = 5;

% output video parameters
params.writeVideo = 0;
params.videoFile = 'C:/position.avi';

% initial state of needle
xcurr.pos = [0; 0; 0];
xcurr.q = RotationMatrixToQuat(eye(3));
xcurr.rho = 60;        % 10 cm roc

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
view(90,0);
axis equal;
hold on;
grid on;

writerObj = [];
if(params.writeVideo)
    writerObj = VideoWriter(params.videoFile);
    open(writerObj);
end

particleHandles = [];
tipFrameHandles = [];
usFrameHandles = [];
for t=0:params.dt:params.simulationTime
    %current command
    uc.v = 10;          %10mm/s
    uc.dc = 0;          %no-duty cycle
    
    %flip it for an s-curve optionally
    if(t > 3 && ~flipped)
        uc.dtheta = pi;
        flipped = 1;
    else
        uc.dtheta = 0;
    end
    
    % save off the previous state to draw the needle segment
    xlast = xcurr;
    %propagate through system dynamics
    xcurr = f(xcurr,uc,params);
    
    % draw the needle segment from x_{t-1}->x_t
    xd = [xlast.pos xcurr.pos]';
    plot3(xd(:,1), xd(:,2), xd(:,3), 'k', 'LineWidth', 2);
    hold on;
    
    % shift our command history down
    u(2:params.n) = u(1:params.n-1);
    % place our current command in u{1}
    u{1} = uc;
    % get previous frames based on current state and u
    xs = propagateNeedleBack(xcurr,u,params);
    
    % if we're far enough along, start generating random US measurements
    measurement = [];
    if(t > 2)
        measurement = generateRandomMeasurement(xcurr, u, params);
        if(~params.particlesInit)
            xp = initializeParticles(xcurr, params);
            params.particlesInit = 1;
        else
            xp = propagateParticles(xp,uc,params);
        end
    end
    
    % draw auxillary information if these params are enabled
    if(params.drawTipFrame && params.drawPastTipFrames)
        tipFrameHandles = drawFrames(1, xs(1:4:end), 20, tipFrameHandles);
    elseif(params.drawTipFrame)
        tipFrameHandles = drawFrames(1, xs(1), 20, tipFrameHandles);
    end
    
    if(params.particlesInit)
        particleHandles = drawParticles(1,xp,params, particleHandles);
    end
    
    if(~isempty(measurement))
        usFrameHandles = drawUSFrame(measurement,params, usFrameHandles);
    end
   
    if(~isempty(writerObj))
        frame = getframe;
        writeVideo(writerObj, frame);
    end
    ylim([-100 100]);
    zlim([-10 100]);
    xlim([-100 100]);
    pause(params.dt/5);
end

close(writerObj);







