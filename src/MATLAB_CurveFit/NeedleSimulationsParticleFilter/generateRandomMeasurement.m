%% state update dynamics
% x = current needle tip state
%   x.pos % current position of needle tip frame 
%   x.q  % current orientation of needle tip 
%   x.rho current radius of curvature
% u = control input
%   u{1}        = current action
%   u{2}        = action 1 timestamp back
%   ...
%   u{n}        = action n timestamps back
%   u{i}.v         = insertion velocity
%   u{i}.dtheta    = rotation about needle's axis
%   u{i}.dc        = duty cycle ratio
%  t =  current time step (seconds)
% params = simulation parameters
% return measurement
%  measurement.pos = [x;y;z] is location of measurement
%  measurement.doppler = doppler response
%  measurement.ful,fbl,fbr,fur = us frame positions
%  measurement.bx = measurement.fbl-measurement.fur
% see ../NeedleSimulation.m for description of parameters
function measurement = generateRandomMeasurement(x, u, t, params)
if(params.doRandomTransducerPositioning)
    axisLoc = mvnrnd(params.axialMu, params.axialSigma);
else
    % start frame "behind" needle tip 
    axisLoc = params.axialMu+params.axialSigma*sin(2*pi*params.axialFrequency*(t-params.particleInitTime)-0);%pi/2);
end

% US frame is ahead of needle tip
if(axisLoc > 0)
    uc = u{1};
    
    %velocity needed to get the distance along axis
    uc.v = axisLoc/params.dt;
    
    %propagate forward by axisLoc mm
    xus = propagateNeedleTip(x,uc,params);
else
    %US frame behind tip
    dist = 0;
    xus = x;
    endid = 1;
    while(dist < abs(axisLoc) && endid < length(u))
        dist = dist+params.dt*u{endid}.v;
        endid=endid+1;
    end
    
    us = u(1:endid);
    xus = propagateNeedleBack(x,us,params);
    xus = xus{endid};
    
    measurement.doppler = lognrnd(params.onNeedleDopplerMu, params.onNeedleDopplerSigma);
end

% orientation perturbation of US frame
orientationPertubation = mvnrnd(params.frameOrientationMu, params.frameOrientationSigma);

%frame orientation
R = QuatToRotationMatrix(xus.q);
Rf = R*Rx(orientationPertubation(1))*Ry(orientationPertubation(2))*Rz(orientationPertubation(3));

measurement.ful = xus.pos-Rf(:,1)*params.ush/2-Rf(:,2)*params.usw/2;
measurement.fbl = xus.pos+Rf(:,1)*params.ush/2-Rf(:,2)*params.usw/2;
measurement.fbr = xus.pos+Rf(:,1)*params.ush/2+Rf(:,2)*params.usw/2;
measurement.fur = xus.pos-Rf(:,1)*params.ush/2+Rf(:,2)*params.usw/2;

if(axisLoc > 0)
    % if we're past the end of the needle tip, then we'll get uniformly distributed
    % measurements about frame
%     measurementNoise = rand(2,1);
%     measurement.pos = measurement.ful+(measurement.fur-measurement.ful)*measurementNoise(1)+...
%         (measurement.fbl-measurement.ful)*measurementNoise(1);
    measurementNoise = mvnrnd(params.measurementOffsetMu, params.measurementOffsetSigma);
    measurement.pos = xus.pos+Rf(:,1)*measurementNoise(1)+Rf(:,2)*measurementNoise(2);
    measurement.doppler = lognrnd(params.offNeedleDopplerMu, params.offNeedleDopplerSigma);
else
    %position measurement
    measurementNoise = mvnrnd(params.measurementOffsetMu, params.measurementOffsetSigma);
    measurement.pos = xus.pos+Rf(:,1)*measurementNoise(1)+Rf(:,2)*measurementNoise(2);
end
measurement.bx = measurement.fur-measurement.ful;  measurement.bx = measurement.bx/norm(measurement.bx);
measurement.by = measurement.fbl-measurement.ful;  measurement.by = measurement.by/norm(measurement.by);
%express measurement in image coordinates (mm)
measurement.uv = [measurement.bx measurement.by]\(measurement.pos-measurement.ful);
assert(sum(abs(measurement.ful+[measurement.bx measurement.by]*measurement.uv-measurement.pos)) < 1e-6);
end