% compute Average position Error
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weights
% xcurr = current needle state
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function error = computeAverageOrientationError(x, xcurr, params)
[pos,~,~,pw] = particleArrays(x);

error = 0;
pc = xcurr.pos;
for i=1:size(pos,1)
error = error + norm(pos(i,:)'-pc)*pw(i);
end
end