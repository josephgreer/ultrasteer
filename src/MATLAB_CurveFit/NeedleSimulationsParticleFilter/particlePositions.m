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
elseif(params.particleFilterMethod == 2)
    pos = particlePositions2(xp, params);
elseif(params.particleFilterMethod == 3)
    pos = particlePositions3(xp, params);
elseif(params.particleFilterMethod == 4)
    pos = particlePositions4(xp, params);
else
    pos = particlePositions100(xp, params);
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

function pos = particlePositions3(xp, params)
pos = cell2mat(xp);
pos = [pos.pos]';
end

function pos = particlePositions4(xp, params)
pos = cell2mat(xp);
pos = [pos.pos]';
end

function pos = particlePositions100(xp, params)
pos = xp{1}.pos';
end