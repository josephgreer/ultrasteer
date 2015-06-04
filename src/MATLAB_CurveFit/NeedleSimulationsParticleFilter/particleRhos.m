% get particle rhos depending on particle filter state
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weights
% xcurr = current needle state
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function rho = particleRhos(xp, params)
if(params.particleFilterMethod == 1)
    rho = particleOrientations1(xp, params);
else
    rho = particleOrientations2(xp, params);
end
end

function rho = particleOrientations1(xp, params);
arrays = cell2mat(xp);
rho = [arrays.rho]';
end

function rho = particleOrientations2(xp, params)
rho = zeros(length(xp),1);
for i=1:length(xp)
    % fit circle to three most recent points
    circ = Circle3([xp{i}.pos{1} xp{i}.pos{2} xp{i}.pos{3}]);
    rho(i) = circ.r;
end
end