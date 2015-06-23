% get particle orientations depending on particle filter state
% x = needle tip state
%   x.pos % position of needle tip frame n timesteps back
%   x.q  % orientation of needle tip frame n timesteps back
%   x.rho radius of curvature (mm)
%   x.w = particle weights
% xcurr = current needle state
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function q = particleOrientations(xp, params)
if(params.particleFilterMethod == 1)
    q = particleOrientations1(xp, params);
elseif(params.particleFilterMethod == 2)
    q = particleOrientations2(xp, params);
elseif(params.particleFilterMethod == 3)
    q = particleOrientations3(xp, params);
elseif(params.particleFilterMethod == 4)
    q = particleOrientations4(xp, params);
else
    q = particleOrientations100(xp, params);
end
end

function q = particleOrientations1(xp, params)
q = zeros(length(xp),4);
arrays = cell2mat(xp);
q = [arrays.q]';
end

function q = particleOrientations2(xp, params)
q = zeros(length(xp),4);
for i=1:length(xp)
    % fit circle to three most recent points
    circ = Circle3([xp{i}.pos{1} xp{i}.pos{2} xp{i}.pos{3}]);
    % calculate frame
    binormal = circ.c-xp{i}.pos{1};
    binormal = binormal/norm(binormal);
    tangent = cross(circ.N, binormal);
    tangent = tangent/norm(tangent);
    
    q(i,:) = RotationMatrixToQuat([normal binormal tangent])';
end
end

function q = particleOrientations3(xp, params)
q = zeros(length(xp), 4);
for i=1:params.np
    q(i,:) = RotationMatrixToQuat(xp{i}.qdist.mu)';
end
end

function q = particleOrientations4(xp, params)
q = RotationMatrixToQuat(xp{1}.qdist.mu)';
if((abs(norm(q) - 1) > 1e-2))
    x = 0;
end
end

function q = particleOrientations100(xp, params)
q = RotationMatrixToQuat(xp{1}.dist.mu)';
if((abs(norm(q) - 1) > 1e-2))
    x = 0;
end
end