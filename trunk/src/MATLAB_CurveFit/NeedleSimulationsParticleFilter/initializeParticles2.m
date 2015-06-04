% intialize particles based on true state of needle 
% this is for "2nd" version of particle filter 
% where a history of points is kept track of and 
% frames/curvatures are generated on the fly based on 
% previous point histroy
% x = needle tip state
%   x.pos{1} % position of needle tip frame
%   x.pos{2} % position of needle tip frame 1 step back
%   ...
%   x.w = particle weight
% u = control input
%   u{1}        = current action
%   u{2}        = action 1 timestamp back
%   ...
%   u{n}        = action n timestamps back
%   u{i}.v         = insertion velocity
%   u{i}.dtheta    = rotation about needle's axis
%   u{i}.dc        = duty cycle ratio
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function xp = initializeParticles2(x, u, params)
xp = {};

pos = cell2mat(x);
pos = [pos.pos]';
pos = repmat(pos, params.np, 1);
posNoise = mvnrnd(params.p2.initPosMu, params.p2.initPosSigma,...
    params.np*params.n);
pos = pos+posNoise;

for p=1:params.np
    xp{p}.pos = mat2cell(pos((p-1)*params.n+1:p*params.n,:)', 3, ones(params.n,1));
    xp{p}.w = 1/params.np;
end

xp = xp';

end