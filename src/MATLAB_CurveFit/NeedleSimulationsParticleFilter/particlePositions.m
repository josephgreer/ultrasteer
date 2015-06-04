% get particle orientations depending on particle filter state
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weights
% xcurr = current needle state
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function pos = particlePositions(xp, params)
if(params.particleFilterMethod == 1)
    pos = particlePositions1(xp, params);
else
    pos = particlePositions2(xp, params);
end
end

function pos = particlePositions1(xp, params)
pos = cell2mat(xp);
pos = [pos.pos]';
end

function pos = particlePositions2(xp, params)
pos = cell2mat(xp);
pos = [pos.pos];
pos = [pos{1:params.n:end}]';
end