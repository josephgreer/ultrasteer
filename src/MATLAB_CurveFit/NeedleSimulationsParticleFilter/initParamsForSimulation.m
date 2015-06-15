%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% initParamsForSimulation
%%% initializes parameter structure for use in needleSimulation.m
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function params = initParamsForSimulation()

addpath('icp');
addpath('spheresurf');

% setup params

%simulation parameters
params.dt = 1/10;                                                           %simulation timestep (s)

% process noise parameters
params.muOrientation = [0; 0; 0];                                           %orientation process noise mean
params.sigmaOrientation = diag(1/2*[pi/1000, pi/1000, pi/1000]);            %orientation process noise std
params.muPos = [0; 0; 0];                                                   %position process noise mean
params.sigmaPos = diag(0*[1 1 1]);                                          %position process noise std
params.muRho = 0;                                                           %needle radius of curvature process noise mean
params.sigmaRho = 3;                                                        %needle radius of curvature process noise std
params.muVelocity = 0;
params.sigmaVelocity = 5;                                                   %velocity sigma
params.simulationTime = 15;                                                 %simulate 10 seconds of insertion

% ultrasound dimensions/parameters
params.usw = 83*630*1e-3;                                                   %physical width of ultrasound frame (mm)
params.ush = 83*480*1e-3;                                                   %phyiscal height of ultrasound frame (mm)
params.axialMu = 0;                                                         %(>0) ahead of needle tip, (<0) behind needle tip 0 is on
params.doRandomTransducerPositioning = 0;                                   % if this parameter is 0 => transducer moves randomly according to parameters axialSigma and axialMu.  Else swep sinusoidally.
params.axialSigma = 10;                                                     %sigma of where the image plane is wrt to needle tip (mm)
params.axialFrequency = 0.3;                                                % number of sweeps over the tip per second
params.frameOrientationMu = [0; 0; 0];                                      %frame orientation mu
params.frameOrientationSigma = diag([pi/1000, pi/1000, pi/1000]);           %frame orientation sigma

% measurement noise parameters
params.measurementOffsetMu = [0; 0];                                        %zero measurement noise offset
params.mpp = 83;                                                            %microns per pixel
params.measurementOffsetSigma = diag([params.mpp*5*1e-3 params.mpp*5*1e-3]);  %5 pixels of measurement noise
params.offNeedleDopplerMu = 0.56;                                           %Doppler strength of measurement off needle distributed according to lognormal dist
params.offNeedleDopplerSigma = 0.75;                                        %Doppler strength of measurement off needle distributed according to lognormal dist
params.onNeedleDopplerMu = 2.33;                                            %Doppler strength of measurement on needle distributed according to lognormal dist
params.onNeedleDopplerSigma = 0.098;                                        %Doppler strength of measurement on needle distributed according to lognormal dist

% particle filter parameters
params.doParticleFilter = 1;                                                %should we particle filter?
params.doMeasurement = 1;                                                   %should we perform measurement step?
params.particleFilterMethod = 1;                                            %particle filter method. see initialize particles for description of each.
params.n = 20;                                                              %only keep track of current state
params.np = 100;                                                            %number of particles
params.neff = 0.5;                                                          %if effective sample size drops below params.neff*parrams.np then resample
params.particlesInit = 0;                                                   %are particles initialized?
params.particleInitTime = 2;                                                %how long until the particle filter gets initialized?

% particle filter method specific parameters

% method 1
params.p1.initOrientationSigma = diag(1/2*[pi/1000, pi/1000, pi/1000]);     %orientation sigma for initial distribution of particles
params.p1.initOrientationMu = [0;0;0];                                      %orientation mu for initial distribution of particles
params.p1.initRhoSigma = 3;                                                 %rho sigma for initial distribution of particles
params.p1.initRhoMu = 0;                                                    %rho mu for initial distribution of particles
params.p1.initPosSigma = diag([5 5 5]);                                     %pos sigma for initial distribution of particles
params.p1.initPosMu = [0; 0; 0];                                            %pos mu for initial distribution of particles

% method 2
params.p2.drawPastPos = 1;                                                  %draw past particle points as well.
params.p2.initPosSigma = diag([5 5 5]);                                     %pos sigma for initial distribution of particles
params.p2.initPosMu = [0; 0; 0];                                            %pos mu for initial distribution of particles
params.p2.pastParticleSpacing = 10;                                         %axial spacing of past positions in (mm)

% method 100
params.p100.initOrientationSigma = diag(1/10*[pi/1000, pi/1000, pi/1000]);  %orientation sigma for initial sigma of kalman filter
params.p100.measurementSigma = diag(1/5*[pi/1000, pi/1000, pi/1000]);       %measurement sigma for orientation
params.p100.minimumMeasurements = 15;                                       %minimum number of measurements before we can generate rotations from measurements


% drawing parameters
params.drawMeasurement = 1; 
params.drawUSFrame = 1;
params.drawTipFrame = 1;
params.drawPastTipFrames = 0;
params.drawParticlePos = 0;
params.drawParticleOrientation = 0;
params.drawParticlesNs = 1;
params.drawExpectedPos = 1;
params.drawExpectedOrientation = 0;
params.drawTruePos = 1;

% output video parameters
params.writeVideo = 0;
params.videoFile = 'C:/position.avi';
end