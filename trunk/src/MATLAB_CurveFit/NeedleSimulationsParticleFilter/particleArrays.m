% creates for each field of the particle structure of all the particles
function [pos rho q] = particleArrays(xp, params)
pos = zeros(length(xp), 3);
rho = zeros(length(xp),1);
q = zeros(length(xp),4);
for i=1:length(xp)
    pos(i,:) = xp{i}.pos';
    rho(i) = xp{i}.rho;
    q(i,:) = xp{i}.q';
end
end