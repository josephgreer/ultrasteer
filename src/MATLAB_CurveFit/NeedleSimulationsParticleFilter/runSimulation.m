%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% runSimulation
%%% inputs:
%%%     runs a simulation with parameters initialized in initParams
%%%     u = commandFcn(t,params) returns commands based on the timestep
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function results = runSimulation(params, commandFcn)
% results structure
results.states = {};
results.estimatedStates = {};


% initial state of needle
xcurr.pos = [0; 0; 0];
xcurr.q = RotationMatrixToQuat(eye(3));
xcurr.rho = 60;        % 10 cm roc

uc.v = 0;               %10 mm/s
uc.dc = 0;              %no duty cycle
uc.dtheta = 0;

u{1} = uc;
u = repmat(u,params.n,1);

% simulation
figure(1);
ylim([-80 80]);
zlim([-10 100]);
xlim([-50 50]);
xlabel('x');
ylabel('y');
zlabel('z');
view(90,0);
axis equal;
hold on;
grid on;

writerObj = [];
if(params.writeVideo)
    writerObj = VideoWriter(params.videoFile);
    open(writerObj);
end

particleHandles = [];
tipFrameHandles = [];
usFrameHandles = [];
for t=0:params.dt:params.simulationTime
    %current command
    uc = commandFcn(t,params);
    
    % save off the previous state to draw the needle segment
    xlast = xcurr;
    %propagate through system dynamics
    xcurr = f(xcurr,uc,params);
    
    % draw the needle segment from x_{t-1}->x_t
    xd = [xlast.pos xcurr.pos]';
    plot3(xd(:,1), xd(:,2), xd(:,3), 'k', 'LineWidth', 2);
    hold on;
    
    % shift our command history down
    u(2:params.n) = u(1:params.n-1);
    % place our current command in u{1}
    u{1} = uc;
    % get previous frames based on current state and u
    xs = propagateNeedleBack(xcurr,u,params);
    
    % if we're far enough along, start generating random US measurements
    measurement = [];
    if(t > params.particleInitTime)
        measurement = generateRandomMeasurement(xcurr, u, params);
        if(~params.particlesInit)
            fakeCurr = xcurr;
            xp = initializeParticles(fakeCurr, params);
            xpe = expectedValueOfParticles(xp,params);
            params.particlesInit = params.doParticleFilter;
        else
            xp = propagateParticles(xp,uc,params);
            pw = measureParticles(xp,u,measurement,params);
            pids = sample(pw, length(xp));
            xpu = {xp{pids}}';
            xpe = expectedValueOfParticles(xp,params);
            xpeu = expectedValueOfParticles(xpu, params);
        end
    end
    
    % draw auxillary information if these params are enabled
    if(params.drawTipFrame && params.drawPastTipFrames)
        tipFrameHandles = drawFrames(1, xs(1:4:end), 20, tipFrameHandles);
    elseif(params.drawTipFrame)
        tipFrameHandles = drawFrames(1, xs(1), 20, tipFrameHandles);
    end
    
    if(params.particlesInit)
        particleHandles = drawParticles(1,xp, xpe,xcurr,params, particleHandles);
        if(exist('xpu'))
            particleHandles = drawParticles(1,xpu, xpe,xcurr, params,particleHandles);
            xp = xpu;
        end
    end
    
    if(~isempty(measurement))
        usFrameHandles = drawUSFrame(measurement,params, usFrameHandles);
    end
    
    if(~isempty(writerObj))
        frame = getframe;
        writeVideo(writerObj, frame);
    end
    ylim([-100 100]);
    zlim([-10 100]);
    xlim([-100 100]);
    pause(params.dt/5);
end

close(writerObj);
end
