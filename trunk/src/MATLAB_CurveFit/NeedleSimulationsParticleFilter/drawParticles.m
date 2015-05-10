% fig = figure index
% xp = needle tip particles
%   xp{i}.pos % position of needle tip frame n timesteps back
%   xp{i}.q  % orientation of needle tip frame n timesteps back
%   xp{i}.rho radius of curvature (mm)
% xpe = expected state
% params = simulation parameters
% see ../NeedleSimulation.m for description of parameters
function handles = drawParticles(fig, xp, xpe,xcurr, params, handles)
if(isempty(handles))
    handles.tipFrames = [];
    handles.expectedTipFrame = [];
    handles.particlePos = [];
    handles.expectedParticlePos = [];
    handles.expectedTipFrame = [];
    handles.currPos = [];
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

if(params.drawExpectedPos)
    if(isempty(handles.expectedParticlePos))
        handles.expectedParticlePos = scatter3(xpe.pos(1),xpe.pos(2),xpe.pos(3),'filled','MarkerFaceColor',[1 1 0]);
    else
        set(handles.expectedParticlePos, 'XData', xpe.pos(1), 'YData', xpe.pos(2), 'ZData', xpe.pos(3));
    end
end

if(params.drawTruePos)
    if(isempty(handles.currPos))
        handles.currPos = scatter3(xcurr.pos(1),xcurr.pos(2),xcurr.pos(3),'filled','MarkerFaceColor',[1 0 0]);
    else
        set(handles.currPos, 'XData', xcurr.pos(1), 'YData', xcurr.pos(2), 'ZData', xcurr.pos(3));
    end
end

if(params.drawParticleOrientation)
    handles.tipFrames = drawFrames(fig, {xp{randi([1 ns]):ns:end}}', 5, handles.tipFrames);
end

if(params.drawExpectedOrientation)
    handles.expectedTipFrame = drawFrames(fig, {xpe}, 5, handles.expectedTipFrame);
end

end