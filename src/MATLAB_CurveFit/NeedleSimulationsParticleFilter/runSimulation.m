%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% runSimulation
%%% inputs:
%%%     runs a simulation with parameters initialized in initParams
%%%     u = commandFcn(t,params) returns commands based on the timestep
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [results, xhist, u] = runSimulation(params, commandFcn)
% results structure
results.states = {};
results.estimatedStates = {};
results.time = [];
results.positionError = [];
results.orientationError = [];
results.particles = [];
results.measurements = [];
results.us = [];


% initial state of needle
xcurr.pos = [0; 0; 0];
xcurr.q = RotationMatrixToQuat(eye(3));
xcurr.rho = 80;        % 10 cm roc
xcurr.w = 1;           % not a particle but using the same propagation routine.

% historic states
xhist = {};

uc.v = 0;               %10 mm/s
uc.dc = 0;              %no duty cycle
uc.dtheta = 0;

u{1} = uc;
u = repmat(u,params.n,1);

% simulation
figure(1);
if(params.drawFloatingFrame)
    subplot(2,2,1:2);
    title('Simulated Insertion');
end
ylim([-40 150]);
zlim([-40 150]);
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
    writerObj = VideoWriter(params.videoFile,'MPEG-4');
    open(writerObj);
end

particleHandles = [];
tipFrameHandles = [];
usFrameHandles = [];
floatingFrameHandles1 = [];
floatingFrameHandles2 = [];

% list of measurements
measurements = {};


for t=0:params.dt:params.simulationTime
    %current command
    uc = commandFcn(t,params);
    
    %propagate through system dynamics
    xcurr = f(xcurr,uc,params,params);
    
    xhist = vertcat({xcurr},xhist);
    if(length(xhist) > params.n)
        xhist(params.n+1:end) = [];
    end
    
    % draw the needle segment from x_{t-1}->x_t
    if(length(xhist) > 1)
        xd = [xhist{2}.pos xcurr.pos]';
        plot3(xd(:,1), xd(:,2), xd(:,3), 'k', 'LineWidth', 2);
        hold on;
    end
    
    % shift our command history down
    u(2:params.n) = u(1:params.n-1);
    % place our current command in u{1}
    u{1} = uc;
    
    % if we're far enough along, start generating random US measurements
    measurement = [];
    if(t > params.particleInitTime)
        measurement = generateRandomMeasurement(xhist, u, t, params);
        measurements = vertcat({measurement}, measurements);
        if(~params.particlesInit)
            fakeCurr = xhist;
            %fakeCurr{1}.rho = xhist{1}.rho-4;
            xp = initializeParticles(fakeCurr, u, params);
            params.particlesInit = params.doParticleFilter&length(measurements)>=getMinimumMeasurements(params);
        else
            xp = propagateParticles(xp,uc,params);
            dts = repmat(params.dt, params.n);
            if(params.doMeasurement)
                xp = measureParticles(xp,u,xhist,dts,measurements,params);
            end
        end
        
        neff = effectiveSampleSize(xp, params);
        xpe = expectedValueOfParticles(xp,params);
        % resample if effective particle size gets too low.
        if(neff < params.neff*params.np)
            xp = resampleParticles(xp, params);
            %display('resample');
        end
        
        %save off estimated state
        results.estimatedStates = vertcat(results.estimatedStates, xpe);
        %save off particles
        results.particles = vertcat(results.particles, {xp});
        % save off orientation error
        results.orientationError = vertcat(results.orientationError, computeAverageOrientationError(xp, xcurr, params));
        % save off pos error
        results.positionError = vertcat(results.positionError, computeAveragePositionError(xp, xcurr, params));
        % save off measurements
        results.measurements = vertcat(results.measurements, {measurement});
    end
    % save off state
    results.states = vertcat(results.states, xcurr);
    % save off time
    results.time = vertcat(results.time, t);
    % save off commands
    results.us = vertcat(results.us, {uc});
    
    % draw auxillary information if these params are enabled
    if(params.drawTipFrame && params.drawPastTipFrames)
        % get previous frames based on current state and u
        xs = propagateNeedleBack(xcurr,u,params);
        tipFrameHandles = drawFrames(1, xs(1:4:end), 20, params, tipFrameHandles);
    elseif(params.drawTipFrame)
        tipFrameHandles = drawFrames(1, {xcurr}, 20, params, tipFrameHandles);
    end
    
    if(params.particlesInit)
        particleHandles = drawParticles(1,xp, xpe,xcurr,params, particleHandles);
    end
    if(params.particlesInit && params.drawFloatingFrame)
        subplot(2,2,3);
        floatingFrameHandles1 = drawFloatingFrame(1,{xcurr}, 20, params, floatingFrameHandles1);
        view(90,0);
        title('True Orientation of Tip');
        subplot(2,2,4);
        floatingFrameHandles2 = drawFloatingFrame(1,{xpe}, 20, params, floatingFrameHandles2);
        view(90,0);
        title('Estimated Orientation of Tip (With Kalman Filter)');
        subplot(2,2,1:2);
    end
    
    if(~isempty(measurement))
        usFrameHandles = drawUSFrame(measurement,params, usFrameHandles);
    end
    
    if(~isempty(writerObj))
    	%if we're writing a video, make it big.
        if(t == 0)
            pos = get(1, 'position');
            set(1, 'position', [pos(1)-150 pos(2)-150 1.5*pos(3) 1.5*pos(4)]);
        end
        if(params.particlesInit)
            frame = getframe(1);
            writeVideo(writerObj, frame);
        end
        %writeVideo(writerObj, frame);
    end
    %pause(params.dt/5);
end

close(writerObj);
end
