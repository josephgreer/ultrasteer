% propagate particles based on true state of needle 
% calls correct propagateParticles routine based on particle
% filter method.
% see ../NeedleSimulation.m for description of parameters
function x = propagateParticles(xp, u, params)
if(params.particleFilterMethod == 1)
    x = propagateParticles1(x,u,params);
else
    x = propagateParticles2(x,u,params);
end
end