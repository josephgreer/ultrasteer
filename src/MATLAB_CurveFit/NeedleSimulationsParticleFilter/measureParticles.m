%% state measurement equation
% u = control input
%   u{1}        = current action
%   u{2}        = action 1 timestamp back
%   ...
%   u{n}        = action n timestamps back
%   u{i}.v         = insertion velocity
%   u{i}.dtheta    = rotation about needle's axis
%   u{i}.dc        = duty cycle ratio
% xhist = true state. 
% xhist{1} = current state
% ...
% xhist{n} = state n steps back
%   This parameter is only passed in for rotation kalman filter
% measurements
%  measurement{1}.pos = [x;y;z] is current location of measurement
%  measurement{1}.doppler = doppler response
%  measurement{1}.ful,fbl,fbr,fur = us frame positions
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function x = measureParticles(xp, u, xtrue, measurements, params)
if(params.particleFilterMethod == 1)
    pw = measureParticles1(xp, u, measurements, params);
elseif(params.particleFilterMethod == 2)
    pw = measureParticles2(xp, u, measurements, params);
elseif(params.particleFilterMethod == 3)
    [xp,pw] = measureParticles3(xp, u, measurements, params);
elseif(params.particleFilterMethod == 4)
    [xp,pw] = measureParticles4(xp, u, measurements, params);
else
    xp = measureParticles100(xp, u, xtrue, measurements, params);
    pw = 1;
end

%now incorporate weights of previous particles
w = particleWeights(xp, params);
w = pw.*w;
w = w/sum(w);
for i=1:length(xp)
    xp{i}.w = w(i);
end
x = xp;
end

% x = needle tip statee state
%   x.pos % position of needle tip frame 
%   x.q  % orientation of needle tip frame 
%   x.rho radius of curvature
function pw = measureParticles1(xp, u, measurements, params)
measurement = measurements{1};
pw = zeros(length(xp),1);
% compute p(z|x)
% z = ((u,v),d)
% (u,v) = image coordinates of measurement
% d = doppler response of measurement
% assume p(d,(u,v)|x) = p(d|x)*p((u,v)|x)
% p(d|x) is our "transducer over tip model"
% compute as follows:  if transducer is over the needle segmenet that
% corresopnds to the particle use on-axis doppler probability distribution
% to compute p(d|x) (this is trained offline)
% if transducer is not over the needle segmenet that cooresponds to the
% particle use off-axis doppler probability dist to compute p(d|x) (again
% trained offline)
% to compute p((u,v)|x), calculate distance in image plane of intersection
% of curve segment and transducer image plane and measurement position.
% then look up the probability of this distance

xsc = propagateNeedleBack(xp{1}, u, params);
xsc = cell2mat(xsc);
xsc = [xsc.pos];

R1 = QuatToRotationMatrix(xp{1}.q);

for i=1:length(xp)
    Ri = QuatToRotationMatrix(xp{i}.q);
    Rdelta = Ri*R1';
    
    %vector pointing from particle to measurement loc
    dr = measurement.pos-xp{i}.pos;
    
    %look at projection of dr onto particle tip frame.  if it's positive
    %then we believe it's in front of this particle (in other words, the
    %ultrasound frame is off the needle in this hypothetical position)
    R = QuatToRotationMatrix(xp{i}.q);
    proj = dr'*R(:,3);
    
    % compute p((u,v)|x), p(d|x)
    p_uvx = 0;
    p_dx = 0;
    
    % does particle intersect the frame?
    offFrame = 1;
    % if image frame is past the end of the particle needle tip, then yes
    if(proj <= 0)
        %before the needle tip of this particle
        % work backwards
        xs = xsc-repmat(xsc(:,1),1,size(xsc,2));
        xs = Rdelta*xs;
        xs = xs+repmat(xp{i}.pos, 1, size(xsc,2));
        
%         xx = propagateNeedleBack(xp{i}, u, params);
%         xx = cell2mat(xx); xx = [xx.pos];
%         
%         assert(norm(abs(xx-xs)) < 1e-5);
        for j=1:length(xs)-1
            dr = xs(:,j+1)-xs(:,j);
            if(norm(dr) < 1e-5)
                continue;
            end
            if(det([-dr measurement.bx measurement.by]) == 0)
                ss = 0;
            end
            % t(2:3) = image coordinates of particle image frame intersection
            % in mm
            t = [-dr measurement.bx measurement.by]\(xs(:,j)-measurement.ful);
            if(sum(zeros(3,1) <= t)==3 && sum(t <= [1;params.usw;params.ush])==3)
                % t(2:3) = intersection of shaft and particle in image coordinates (mm)
                suv = t(2:3);
                
                % measurement.uv = measurement in image coordinaates in (mm)
                % p((u,v)|d,x) ~ truncated gaussian centered at shaft particle interesection
                % calculate limits for truncated gaussian
                a = suv-[params.usw;params.ush]; % if shaft (u,v) - (u,v) < shaft (u,v) - br, then  (u,v) > br
                b = suv; % if shaft (u,v) - (u,v) > shaft (u,v), then (u,v) < (0,0) 
                
                duv = suv-measurement.uv;
                
                pin = sigmf(measurement.doppler, [params.sigA, params.sigC]);
                p_uvx = pin*truncatedIndependentGaussianPdf(duv, zeros(2,1), diag(params.p1.uvOffsetSigma),...,
                    a,b) + ...
                    (1-pin)*(1/(params.ush*params.usw));
                
                % p(d|x) in case particle intersects frame
                p_dx = lognpdf(measurement.doppler, params.onNeedleDopplerMu, params.onNeedleDopplerSigma);
                offFrame = 0;
                break;
            end
        end
    end
    
    % particle doesn't intersect image frame
    if(offFrame)
        p_uvx = 1/(params.ush*params.usw);
        p_dx = lognpdf(measurement.doppler, params.offNeedleDopplerMu, params.offNeedleDopplerSigma);
    end
    
    pw(i) = p_dx*p_uvx;
end

% in case all probabilities are exceedingly low!
if(sum(pw) > 0)
    pw = pw/sum(pw);
else
    pw = ones(size(pw))/params.np;
end

end


% x = needle tip statee state
%   x.pos % position of needle tip frame 
%   x.q  % orientation of needle tip frame 
%   x.rho radius of curvature
function pw = measureParticles2(xp, u, measurements, params)
measurement = measurements{1};
pw = zeros(length(xp),1);
% compute p(z|x)
% z = ((u,v),d)
% (u,v) = image coordinates of measurement
% d = doppler response of measurement
% assume p(d,(u,v)|x) = p(d|x)*p((u,v)|x)
% p(d|x) is our "transducer over tip model"
% compute as follows:  if transducer is over the needle segmenet that
% corresopnds to the particle use on-axis doppler probability distribution
% to compute p(d|x) (this is trained offline)
% if transducer is not over the needle segmenet that cooresponds to the
% particle use off-axis doppler probability dist to compute p(d|x) (again
% trained offline)
% to compute p((u,v)|x), calculate distance in image plane of intersection
% of curve segment and transducer image plane and measurement position.
% then look up the probability of this distance
for i=1:length(xp)
    %vector pointing from particle to measurement loc
    dr = measurement.pos-xp{i}.pos{1};
    
    %look at projection of dr onto particle tip frame.  if it's positive
    %then we believe it's in front of this particle (in other words, the
    %ultrasound frame is off the needle in this hypothetical position)
    R = QuatToRotationMatrix(xp{i}.q);
    proj = dr'*R(:,3);
    
    % compute p((u,v)|x), p(d|x)
    p_uvx = 0;
    p_dx = 0;
    
    % does particle intersect the frame?
    offFrame = 1;
    % if image frame is past the end of the particle needle tip, then yes
    if(proj <= 0)
        %before the needle tip of this particle
        % work backwards
        xs = propagateNeedleBack(xp{i},u,params);
        for j=1:length(xs)-1
            dr = xs{j+1}.pos-xs{j}.pos;
            if(det([-dr measurement.bx measurement.by]) == 0)
                ss = 0;
            end
            % t(2:3) = image coordinates of particle image frame intersection
            % in mm
            t = [-dr measurement.bx measurement.by]\(xs{j}.pos-measurement.ful);
            if(sum(zeros(3,1) <= t)==3 && sum(t <= [1;params.usw;params.ush])==3)
                % t(2:3) = intersection of shaft and particle in image coordinates (mm)
                suv = t(2:3);
                
                % measurement.uv = measurement in image coordinaates in (mm)
                % p((u,v)|d,x) ~ truncated gaussian centered at shaft particle interesection
                % calculate limits for truncated gaussian
                a = suv-[params.usw;params.ush]; % if shaft (u,v) - (u,v) < shaft (u,v) - br, then  (u,v) > br
                b = suv; % if shaft (u,v) - (u,v) > shaft (u,v), then (u,v) < (0,0) 
                
                duv = suv-measurement.uv;
                
                pin = sigmf(measurement.doppler, [params.sigA, params.sigC]);
                p_uvx = pin*truncatedIndependentGaussianPdf(duv, zeros(2,1), diag(params.measurementOffsetSigma),...,
                    a,b) + ...
                    (1-pin)*(1/(params.ush*params.usw));
                
                % p(d|x) in case particle intersects frame
                p_dx = lognpdf(measurement.doppler, params.onNeedleDopplerMu, params.onNeedleDopplerSigma);
                offFrame = 0;
                break;
            end
        end
    end
    
    % particle doesn't intersect image frame
    if(offFrame)
        p_uvx = 1/(params.ush*params.usw);
        p_dx = lognpdf(measurement.doppler, params.offNeedleDopplerMu, params.offNeedleDopplerSigma);
    end
    
    pw(i) = p_dx*p_uvx;
end

% in case all probabilities are exceedingly low!
if(sum(pw) > 0)
    pw = pw/sum(pw);
else
    pw = ones(size(pw))/params.np;
end

end



% x = needle tip kalman filter
% x.dist = distribution of kalman filter
% x.rho = current curvature
% x.w = weight (which is only used for particle filter)
function [x,pw] = measureParticles3(xp, u, measurements, params)


R1 = xp{1}.qdist.mu;
xcurr.q = RotationMatrixToQuat(R1);
xcurr.pos = xp{1}.pos;
xcurr.rho = xp{1}.rho;
xcurr.w = 1;

xsc = propagateNeedleBack(xcurr, u, params);
xsc = cell2mat(xsc);
xsc = [xsc.pos];

pw = zeros(params.np,1);

measurement = measurements{1};

for i=1:params.np
    Rprior = xp{i}.qdist.mu;
    
    Rdelta = Rprior*R1';
    
    xs = xsc-repmat(xsc(:,1),1,size(xsc,2));
    xs = Rdelta*xs;
    xs = xs+repmat(xp{i}.pos, 1, size(xsc,2));
    
    % if we don't have enough measurements yet, then just use true quaternion
    dR = zeros(3,3);
    if(length(measurements) >= params.p3.minimumMeasurements)
        dR = optimalRotationForHistory(xs', measurements, params);
    else
        assert(0);
    end
    
    if(norm(SO3HatInverse(SO3Log(dR))) > 0.5)
        yep = 0;
    end
    
    
    Rmeas = dR*Rprior;
    
    % zero measurement noise? just use measurement then
    if(det(params.p3.measurementSigma) < 1e-10)
        x{i}.qdist = SO3Gaussian(Rmeas, zeros(3,3));
    else
        errorVec = diag(params.p3.measurementSigma);
        measSigma = diag(errorVec);
        K = xp{i}.qdist.sigma*inv(xp{i}.qdist.sigma+measSigma);
        sigmaC = (eye(3)-K)*xp{i}.qdist.sigma;
        v = SO3HatInverse(SO3Log(Rprior'*Rmeas));
        Rc = Rprior*SO3Exp(K*v);
        x{i}.qdist = SO3Gaussian(Rc, sigmaC);
        if(abs(det(Rc)-1) > params.errorEpsilon)
            x = 0;
        end
    end
    x{i}.rho = xp{i}.rho;
    x{i}.pos = xp{i}.pos;
    
    
    
    %vector pointing from particle to measurement loc
    dr = measurement.pos-xp{i}.pos;
    
    %look at projection of dr onto particle tip frame.  if it's positive
    %then we believe it's in front of this particle (in other words, the
    %ultrasound frame is off the needle in this hypothetical position)
    R = x{i}.qdist.mu;
    proj = dr'*R(:,3);
    
    Rdelta = R*R1';
    
    % compute p((u,v)|x), p(d|x)
    p_uvx = 0;
    p_dx = 0;
    
    % does particle intersect the frame?
    offFrame = 1;
    % if image frame is past the end of the particle needle tip, then yes
    if(proj <= 0)
        %before the needle tip of this particle
        % work backwards
        xs = xsc-repmat(xsc(:,1),1,size(xsc,2));
        xs = Rdelta*xs;
        xs = xs+repmat(xp{i}.pos, 1, size(xsc,2));
        
        %         xx = propagateNeedleBack(xp{i}, u, params);
        %         xx = cell2mat(xx); xx = [xx.pos];
        %
        %         assert(norm(abs(xx-xs)) < 1e-5);
        for j=1:length(xs)-1
            dr = xs(:,j+1)-xs(:,j);
            if(norm(dr) < 1e-5)
                continue;
            end
            if(det([-dr measurement.bx measurement.by]) == 0)
                ss = 0;
            end
            % t(2:3) = image coordinates of particle image frame intersection
            % in mm
            t = [-dr measurement.bx measurement.by]\(xs(:,j)-measurement.ful);
            if(sum(zeros(3,1) <= t)==3 && sum(t <= [1;params.usw;params.ush])==3)
                % t(2:3) = intersection of shaft and particle in image coordinates (mm)
                suv = t(2:3);
                
                % measurement.uv = measurement in image coordinaates in (mm)
                % p((u,v)|d,x) ~ truncated gaussian centered at shaft particle interesection
                % calculate limits for truncated gaussian
                a = suv-[params.usw;params.ush]; % if shaft (u,v) - (u,v) < shaft (u,v) - br, then  (u,v) > br
                b = suv; % if shaft (u,v) - (u,v) > shaft (u,v), then (u,v) < (0,0)
                
                duv = suv-measurement.uv;
                
                pin = sigmf(measurement.doppler, [params.sigA, params.sigC]);
                p_uvx = pin*truncatedIndependentGaussianPdf(duv, zeros(2,1), diag(params.measurementOffsetSigma),...,
                    a,b) + ...
                    (1-pin)*(1/(params.ush*params.usw));
                
                % p(d|x) in case particle intersects frame
                p_dx = lognpdf(measurement.doppler, params.onNeedleDopplerMu, params.onNeedleDopplerSigma);
                offFrame = 0;
                break;
            end
        end
    end
    
    % particle doesn't intersect image frame
    if(offFrame)
        p_uvx = 1/(params.ush*params.usw);
        p_dx = lognpdf(measurement.doppler, params.offNeedleDopplerMu, params.offNeedleDopplerSigma);
    end
    
    pw(i) = p_dx*p_uvx;
    x{i}.w = xp{i}.w;
   
end

% in case all probabilities are exceedingly low!
if(sum(pw) > 0)
    pw = pw/sum(pw);
else
    pw = ones(size(pw))/params.np;
end

end



% x = needle tip kalman filter
% x.dist = distribution of kalman filter
% x.rho = current curvature
% x.w = weight (which is only used for particle filter)
function [x,pw] = measureParticles4(xp, u, measurements, params)


R1 = xp{1}.qdist.mu;
xcurr.q = RotationMatrixToQuat(R1);
xcurr.pos = xp{1}.pos;
xcurr.rho = xp{1}.rho;
xcurr.w = 1;

xsc = propagateNeedleBack(xcurr, u, params);
xsc = cell2mat(xsc);
xsc = [xsc.pos];
xsc = xsc-repmat(xsc(:,1),1,size(xsc,2));

pw = zeros(params.np,1);

measurement = measurements{1};

qdist = xp{1}.qdist;

for i=1:params.np
    xs = xsc+repmat(xp{i}.pos, 1, size(xsc,2));
    
    %vector pointing from particle to measurement loc
    dr = measurement.pos-xp{i}.pos;
    
    %look at projection of dr onto particle tip frame.  if it's positive
    %then we believe it's in front of this particle (in other words, the
    %ultrasound frame is off the needle in this hypothetical position)
    proj = dr'*R1(:,3);
    
    % compute p((u,v)|x), p(d|x)
    p_uvx = 0;
    p_dx = 0;
    
    % does particle intersect the frame?
    offFrame = 1;
    % if image frame is past the end of the particle needle tip, then yes
    if(proj <= 0)
        for j=1:length(xs)-1
            dr = xs(:,j+1)-xs(:,j);
            if(det([-dr measurement.bx measurement.by]) == 0)
                ss = 0;
            end
            % t(2:3) = image coordinates of particle image frame intersection
            % in mm
            t = [-dr measurement.bx measurement.by]\(xs(:,j)-measurement.ful);
            if(sum(zeros(3,1) <= t)==3 && sum(t <= [1;params.usw;params.ush])==3)
                % t(2:3) = intersection of shaft and particle in image coordinates (mm)
                suv = t(2:3);
                
                % measurement.uv = measurement in image coordinaates in (mm)
                % p((u,v)|d,x) ~ truncated gaussian centered at shaft particle interesection
                % calculate limits for truncated gaussian
                a = suv-[params.usw;params.ush]; % if shaft (u,v) - (u,v) < shaft (u,v) - br, then  (u,v) > br
                b = suv; % if shaft (u,v) - (u,v) > shaft (u,v), then (u,v) < (0,0)
                
                pin = sigmf(measurement.doppler, [params.sigA, params.sigC]);
                p_uvx = pin*truncatedIndependentGaussianPdf(duv, zeros(2,1), diag(params.measurementOffsetSigma),...,
                    a,b) + ...
                    (1-pin)*(1/(params.ush*params.usw));
                
                % p(d|x) in case particle intersects frame
                p_dx = lognpdf(measurement.doppler, params.onNeedleDopplerMu, params.onNeedleDopplerSigma);
                offFrame = 0;
                break;
            end
        end
    end
    
    % particle doesn't intersect image frame
    if(offFrame)
        p_uvx = 1/(params.ush*params.usw);
        p_dx = lognpdf(measurement.doppler, params.offNeedleDopplerMu, params.offNeedleDopplerSigma);
    end
    
    pw(i) = p_dx*p_uvx;
    
    x{i}.rho = xp{i}.rho;
    x{i}.pos = xp{i}.pos;
    x{i}.w = xp{i}.w;
end

% in case all probabilities are exceedingly low!
if(sum(pw) > 0)
    pw = pw/sum(pw);
else
    pw = ones(size(pw))/params.np;
end


%now incorporate weights of previous particles
w = particleWeights(xp, params);
w = pw.*w;
w = w/sum(w);

pos = particlePositions(xp, params);
ep = w'*pos;

   
xs = xsc+repmat(ep', 1, size(xsc,2));
% if we don't have enough measurements yet, then just use true quaternion
dR = zeros(3,3);
if(length(measurements) >= params.p4.minimumMeasurements)
    dR = optimalRotationForHistory(xs', measurements, params);
else
    assert(0);
end

if(norm(SO3HatInverse(SO3Log(dR))) > 0.5)
    yep = 0;
end

Rprior = R1;
Rmeas = dR*Rprior;

% zero measurement noise? just use measurement then
if(det(params.p4.measurementSigma) < 1e-10)
    x{1}.qdist = SO3Gaussian(Rmeas, zeros(3,3));
else
    errorVec = diag(params.p4.measurementSigma);
    measSigma = diag(errorVec);
    K = qdist.sigma*inv(qdist.sigma+measSigma);
    sigmaC = (eye(3)-K)*qdist.sigma;
    v = SO3HatInverse(SO3Log(Rprior'*Rmeas));
    Rc = Rprior*SO3Exp(K*v);
    for i=1:params.np
        x{i}.qdist = SO3Gaussian(Rc, sigmaC);
    end
    if(abs(det(Rc)-1) > params.errorEpsilon)
        x = 0;
    end
end

end



% x = needle tip kalman filter
% x.dist = distribution of kalman filter
% x.rho = current curvature
% x.w = weight (which is only used for particle filter)
function x = measureParticles100(xp, u, xtrue, measurements, params)
xcurr.q = RotationMatrixToQuat(xp{1}.dist.mu);
xcurr.pos = xtrue{1}.pos;
xcurr.rho = xp{1}.rho;
xcurr.w = 1;

xhist = propagateNeedleBack(xcurr, u, params);

Rprior = QuatToRotationMatrix(xcurr.q);


xsc = cell2mat(xhist);
xsc = [xsc.pos]';
% if we don't have enough measurements yet, then just use true quaternion
deltaR = zeros(3,3);
if(length(measurements) >= params.p100.minimumMeasurements)
    deltaR = optimalRotationForHistory(xsc, measurements, params);
else
    deltaR = QuatToRotationMatrix(xtrue{1}.q)*Rprior';
    x{1}.rho = xp{1}.rho;
    x{1}.w = 1;
    x{1}.pos = xcurr.pos;
    x{1}.dist = SO3Gaussian(QuatToRotationMatrix(xtrue{1}.q), params.p100.initOrientationSigma);
    return;
end

if(norm(SO3HatInverse(SO3Log(deltaR))) > 0.5)
    yep = 0;
end


Rmeas = deltaR*Rprior;

% zero measurement noise? just use measurement then
if(det(params.p100.measurementSigma) < 1e-10)
    x{1}.dist = SO3Gaussian(Rmeas, zeros(3,3));
else
    errorVec = diag(params.p100.measurementSigma);
    measSigma = diag(errorVec);
    K = xp{1}.dist.sigma*inv(xp{1}.dist.sigma+measSigma);
    sigmaC = (eye(3)-K)*xp{1}.dist.sigma;
    v = SO3HatInverse(SO3Log(Rprior'*Rmeas));
    Rc = Rprior*SO3Exp(K*v);
    x{1}.dist = SO3Gaussian(Rc, sigmaC);
    if(abs(det(Rc)-1) > params.errorEpsilon)
        x = 0;
    end
end
x{1}.rho = xp{1}.rho;
x{1}.w = 1;
x{1}.pos = xcurr.pos;
end

function drawMeasurementInformationForParticle(xp, xs, u, measurement, params, i)
figure(2);
hold on;

view(3);
view(240, 30);
drawUSFrame(measurement,params,[]);
xss = propagateNeedleBack(xp{i},u,params);
xsc = cell2mat(xss); xsc = [xsc.pos]';
scatter3(xsc(:,1), xsc(:,2), xsc(:,3), 'r');
xs = xs';
scatter3(xs(:,1), xs(:,2), xs(:,3), 'b');
drawFrames(2, xss(1), 20,params,[]);
axis equal;
grid on;
ylim([-100 100]);
zlim([-10 100]);
xlim([-100 100]);

close(2);
end