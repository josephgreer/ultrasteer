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
results.time = [];


% initial state of needle
xcurr.pos = [0; 0; 0];
xcurr.q = RotationMatrixToQuat(eye(3));
xcurr.rho = 60;        % 10 cm roc
xcurr.w = 1;           % not a particle but using the same propagation routine.

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
        measurement = generateRandomMeasurement(xcurr, u, t, params);
        if(~params.particlesInit)
            fakeCurr = xcurr;
            xp = initializeParticles(fakeCurr, params);
            params.particlesInit = params.doParticleFilter;
        else
            xp = propagateParticles(xp,uc,params);
            if(params.doMeasurement)
                xp = measureParticles(xp,u,measurement,params);
            end
        end
        
        neff = effectiveSampleSize(xp, params);
        xpe = expectedValueOfParticles(xp,params);
        % resample if effective particle size gets too low.
        if(neff < params.neff*params.np)
            xp = resampleParticles(xp, params);
        end
        
        % save off state
        results.states = vertcat(results.states, xcurr);
        %save off estimated state
        results.estimatedStates = vertcat(results.estimatedStates, xpe);
        % save off time
        results.time = vertcat(results.time, t);
    end
    
    % draw auxillary information if these params are enabled
    if(params.drawTipFrame && params.drawPastTipFrames)
        tipFrameHandles = drawFrames(1, xs(1:4:end), 20, tipFrameHandles);
    elseif(params.drawTipFrame)
        tipFrameHandles = drawFrames(1, xs(1), 20, tipFrameHandles);
    end
    
    if(params.particlesInit)
        particleHandles = drawParticles(1,xp, xpe,xcurr,params, particleHandles);
    end
    
    if(~isempty(measurement))
        usFrameHandles = drawUSFrame(measurement,params, usFrameHandles);
    end
    
    if(~isempty(writerObj))
    	% if we're writing a video, make it big.
        if(t == 0)
            pos = get(1, 'position');
            set(1, 'position', [pos(1)-150 pos(2)-150 1.5*pos(3) 1.5*pos(4)]);
        end
        frame = getframe;
        writeVideo(writerObj, frame);
        writeVideo(writerObj, frame);
    end
    pause(params.dt/5);
end

close(writerObj);
end
