% intialize particles based on true state of needle 
% calls correct initializeParticlesRoutine based on particle
% filter method.
% see ../NeedleSimulation.m for description of parameters
function xp = initializeParticles(x, u, params)
if(params.particleFilterMethod == 1)
    xp = initializeParticles1(x,u,params);
else
    xp = initializeParticles2(x,u,params);
end
end