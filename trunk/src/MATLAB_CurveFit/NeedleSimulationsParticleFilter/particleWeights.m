% get particle weights
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weights
% xcurr = current needle state
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function w = particleWeights(xp, params)
if(params.particleFilterMethod == 1)
    w = particleWeights1(xp, params);
elseif(params.particleFilterMethod == 2)
    w = particleWeights2(xp, params);
else
    w = particleWeights100(xp, params);
end
end

function w = particleWeights1(xp, params)
w = cell2mat(xp);
w = [w.w]';
end

function w = particleWeights2(xp, params)
w = cell2mat(xp);
w = [w.w]';
end

function w = particleWeights100(xp, params)
w = cell2mat(xp);
w = [w.w]';
end