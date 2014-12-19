% fig = figure index
% xp = needle tip particles
%   xp{i}.pos % position of needle tip frame n timesteps back
%   xp{i}.q  % orientation of needle tip frame n timesteps back
%   xp{i}.rho radius of curvature (mm)
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function handles = drawParticles(fig, xp, params, handles)
if(isempty(handles))
    handles.tipFrames = [];
    handles.particlePos = [];
end
ns = params.drawParticlesNs;
if(params.drawParticlePos)
    pos = zeros(round(length(xp)/ns),3);
    for p=1:round(length(xp)/ns)
        pos(p,:) = xp{p*ns}.pos';
    end
    if(isempty(handles.particlePos))
        handles.particlePos = scatter3(pos(:,1),pos(:,2),pos(:,3),'filled','MarkerFaceColor',[0 0.4 0]);
    else
        set(handles.particlePos, 'XData', pos(:,1), 'YData', pos(:,2), 'ZData', pos(:,3));
    end
end

if(params.drawParticleOrientation)
    handles.tipFrames = drawFrames(fig, {xp{1:ns:end}}', 5, handles.tipFrames);
end

end