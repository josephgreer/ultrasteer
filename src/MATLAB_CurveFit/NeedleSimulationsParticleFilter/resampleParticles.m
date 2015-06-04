% reset particle Weights
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weights
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function xp = resampleParticles(x, params)
pw = particleWeights(x);
pids = sample(pw, length(x));
xp = {x{pids}}';
xp = resetWeights(xp, params);
end