%% calculate expected values of particles
% x = needle tip statee state
%   x.pos % position of needle tip frame 
%   x.q  % orientation of needle tip frame 
%   x.rho radius of curvature
function x = expectedValueOfParticles(xp, params)
[pos rho q] = particleArrays(xp,params);

x.pos = mean(pos,1)';
x.rho = mean(rho);
x.q = [0;0;0;0];
end