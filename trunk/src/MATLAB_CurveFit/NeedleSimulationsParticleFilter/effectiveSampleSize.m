% restet particle Weights
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weights
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function neff = effectiveSampleSize(x, params)
pw = particleWeights(x, params);
neff = 1/(w'*w);
end