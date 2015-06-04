%% calculate expected values of particles
% x = needle tip statee state
%   x.pos % position of needle tip frame 
%   x.q  % orientation of needle tip frame 
%   x.rho radius of curvature
function x = expectedValueOfParticles(xp, params)
pos = particlePositions(xp, params);
rho = particleRhos(xp, params);
q = particleOrientations(xp, params);
w = particleWeights(xp, params);

x.rho = w'*rho;
x.pos = w'*pos;
x.pos = x.pos';
x.q = RotationMatrixToQuat(SO3Mean(q,w,1e-4));
end