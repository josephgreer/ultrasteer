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
function pw = measureParticles(xp, u, measurement, params)
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
    
    % p(doppler | x)
    if(proj > 0)
        % past the end of the tip
        p_dx = lognpdf(measurement.doppler, params.offNeedleDopplerMu, params.offNeedleDopplerSigma);
    else
        p_dx = lognpdf(measurement.doppler, params.onNeedleDopplerMu, params.onNeedleDopplerSigma);
    end
    % compute p((u,v)|x)
    p_uvx = 0;
    if(proj <= 0)
        %before the needle tip of this particle
        % work backwards
        xs = propagateNeedleBack(xp{i},u,params);
        for j=1:length(xs)-1
            dr = xs{j+1}.pos-xs{j}.pos;
            if(det([-dr measurement.bx measurement.by]) == 0)
                ss = 0;
            end
            t = [-dr measurement.bx measurement.by]\(xs{j}.pos-measurement.ful);
            if(0 <= t(1) && t(1) <= 1)
                duv = t(2:3)-measurement.uv;
                p_uvx = mvnpdf(duv, params.measurementOffsetMu, params.measurementOffsetSigma);
                break;
            end
        end
    else
        %after the needle tip of this particle
        dr = R(:,3);
        t = [-dr measurement.bx measurement.by]\(xp{i}.pos-measurement.ful);;
        duv = t(2:3)-measurement.uv;
        p_uvx = mvnpdf(duv, params.measurementOffsetMu, params.measurementOffsetSigma);
    end
    
    pw(i) = p_dx*p_uvx;
end
pw = pw/sum(pw);
end