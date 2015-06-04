%% state update dynamics
% x = needle tip statee state
%   x.pos % position of needle tip frame 
%   x.q  % orientation of needle tip frame 
%   x.rho radius of curvature
% u = control input
%   u{1}        = current action
%   u{2}        = action 1 timestamp back
%   ...
%   u{n}        = action n timestamps back
%   u{i}.v         = insertion velocity
%   u{i}.dtheta    = rotation about needle's axis
%   u{i}.dc        = duty cycle ratio
% measurement
%  measurement.pos = [x;y;z] is location of measurement
%  measurement.doppler = doppler response
%  measurement.ful,fbl,fbr,fur = us frame positions
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function x = measureParticles(xp, u, measurement, params)
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
                p_uvx = truncatedIndependentGaussianPdf(duv, zeros(2,1), diag(params.measurementOffsetSigma),...,
                    a,b);
                
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
    pw = ones(size(pw));
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

function drawMeasurementInformationForParticle(xp, u, measurement, params, i)
figure(2);

view(3);
view(240, 30);
drawUSFrame(measurement,params,[]);
xs = propagateNeedleBack(xp{i},u,params);
drawFrames(2, xs(1), 20,[]);
axis equal;
grid on;
ylim([-100 100]);
zlim([-10 100]);
xlim([-100 100]);

close(2);
end