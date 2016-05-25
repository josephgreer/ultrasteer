clear; clc; close all;

rng(3);

nActuators = 3;                             % number of turning actuators
actuatorSpacing = 2*pi/nActuators;          % spacing between turning actuators
firstAct = 0;                            % angle of first actuator
strengthMean = 400;                         % mean strength
strengthSigma = 0;                         % strength standard deviation

handles = [];

eps = 1e-6;

actuatorThetas = [0:nActuators-1]'*actuatorSpacing + firstAct;
strengths = strengthSigma*randn(nActuators,1)+strengthMean;

nPoints = 100;

J = [];
for i=1:nActuators
    J = [J strengths(i)*[cos(actuatorThetas(i)); sin(actuatorThetas(i))]];
end

figure;
hold on;
daspect([1 1 1]);
pos = [0; 0];
q = ones(nActuators, 1);

drawWorkspace(actuatorThetas, strengths);

xlim([-800 800]);
ylim([-800 800]);

useLeastSquares = false;

deltaStrength = strengthMean;
for i=1:nPoints
    handles = drawRobot(handles, pos, actuatorThetas, q);
    theta = unifrnd(0, 2*pi);
    delta_x = unifrnd(0,deltaStrength)*[cos(theta); sin(theta)];
    
    if(~exist('quiverHandle'))
        quiverHandle = quiver(pos(1), pos(2), delta_x(1), delta_x(2));
    end
    quiverHandle.XData = pos(1); quiverHandle.YData = pos(2);
    quiverHandle.UData = delta_x(1); quiverHandle.VData = delta_x(2);
    pause(0.1);
    
    delta_x_desired = delta_x;
    
    
    if(useLeastSquares)
        delta_q = BVLS(J,delta_x,-q,1-q);
        delta_x_actual = J*delta_q;
        q = q+delta_q;
        if(~all(0 <= q & q <= 1))
            yep = 0;
        end
    else
        done = false;
        delta_x_actual = zeros(size(delta_x));
        while(norm(delta_x) > 1e-6)
            theta = mod(atan2(delta_x(2), delta_x(1)),2*pi);
            
            %dq_test2 = lsqlin(J,delta_x, [], [], [], [], -q, 1-q);
            
            % actuators we can inflate more
            inflateActuators =  find(q < 1);
            % actuators we can deflate more
            deflateActuators = find(q > 0);
            
            thetas = [actuatorThetas(inflateActuators); mod(actuatorThetas(deflateActuators)+pi,2*pi)];
            actuatorIdxs = [inflateActuators; -deflateActuators];
            
            dists = abs(thetas-theta);
            dists = min(dists, 2*pi-dists);
            
            [dists, qidxs] = sort(dists);
            currActs = [abs(actuatorIdxs(qidxs(1))) abs(actuatorIdxs(qidxs(2)))];
            signedActs = [sign(actuatorIdxs(qidxs(1))) sign(actuatorIdxs(qidxs(2)))];
            
            % can't find two actuators aligned with desired displacement
            if((signedActs(1)*dot(J(:,currActs(1)), delta_x) < 0 && signedActs(2)*dot(J(:, currActs(2)), delta_x) < 0) || (currActs(1) == currActs(2)))
                break;
            end
            
            JJ = J(:,currActs);
            
            delta_q = JJ\delta_x;
            
            q_try = q(currActs)+delta_q;
            
            % is this actuator above 1? mask
            q_lim_up = q_try-1;
            q_lim_up = max(q_lim_up, 0);
            q_lim_up(find(q_lim_up > 0)) = 1;
            
            % is this actuator below 0? mask
            q_lim_down = -q_try;
            q_lim_down = max(q_lim_down, 0);
            q_lim_down(find(q_lim_down > 0)) = 1;
            
            % how far up and down can we go?
            res_up = q_lim_up.*(1-q(currActs));
            res_down = q_lim_down.*q(currActs);
            
            % we're going to find the minimum scaling factor so
            % set ones that are not problematic to really high
            res_up(find(q_lim_up == 0)) = 1e10;
            res_down(find(q_lim_down == 0)) = 1e10;
            
            scale_up = abs(res_up./delta_q);
            scale_down = abs(res_down./delta_q);
            
            scale = min(min(scale_up),min(scale_down));
            
            % just in case nothing needs to be scaled
            scale = min(scale, 1);
            if(scale == 0)
                break;
            end
            
            delta_q = delta_q*scale;
            q(currActs) = q(currActs)+delta_q;
            
            delta_x = delta_x-(JJ*delta_q);
            delta_x_actual = delta_x_actual+(JJ*delta_q);
        end
    end
    
    pos = pos+delta_x_actual;
    display(sprintf('{%f, %f, %f, %f, %f},', delta_x_desired(1), delta_x_desired(2),...
        q(1), q(2), q(3)));
    %pause(1);
end

