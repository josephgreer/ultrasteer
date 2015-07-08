%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Plot particle filter results
%%% Inputs:
%%%     results: defined in runSimulation.m
%%%     figureNumber: figure number
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [posErrors, orErrors] = plotParticleFilterResults(results, figureNumber, params)
firsti = find(results.time > params.particleInitTime,1,'first');
states = cell2mat({results.states{firsti:end}});
estimatedStates = cell2mat(results.estimatedStates);
truePos = [states.pos]';
estimatedPos = [estimatedStates.pos]';
figure(figureNumber);
annotation('textbox', [0 0.9 1 0.1], ...
    'String', 'Tip position', ...
    'EdgeColor', 'none', ...
    'HorizontalAlignment', 'center');
% plot position and estimated position over time
coordLabels = {'x coord', 'y coord', 'z coord'};
for i=1:3
subplot(2,2,i);
hold on;

time = results.time(firsti:end);
plot(time, estimatedPos(:,i));
plot(time, truePos(:,i), 'r--');
xlabel('time');
ylabel(coordLabels{i});
end

% quaternion representing tip frame
% [q1 q2 q3 ... qn]
tipFrameQs = [states.q];
% error vector between tip frame and particle filter estimate represented
% in tip frame
errors = [];
for i=1:size(tipFrameQs,2)
  errors = vertcat(errors,(inv(QuatToRotationMatrix(tipFrameQs(:,i)))*((estimatedPos(i,:)-truePos(i,:))'))');
end
e2 = errors.^2;
posErrors = mean(e2,1);

% now plot errors
figure(figureNumber+1);
annotation('textbox', [0 0.9 1 0.1], ...
    'String', 'Tip position error in tip frame', ...
    'EdgeColor', 'none', ...
    'HorizontalAlignment', 'center');
coordLabels = {'x coord', 'y coord', 'z coord'};
for i=1:3
subplot(2,2,i);
hold on;
plot(time, errors(:,i));
xlabel('time');
ylabel(coordLabels{i});
end

figure;
plot(time, results.orientationError);
xlabel('time');
ylabel('orientationError');
orErrors = mean(results.orientationError);

figure;
N = length(results.particles);
rhosTrue = zeros(N,1);
rhosEst = zeros(N,1);
for i=1:N
    rhosTrue(i) = results.states{i}.rho;
    xpe = expectedValueOfParticles(results.particles{i}, params);
    rhosEst(i) = xpe.rho;
end
plot(time, rhosTrue,'b');
hold on;
plot(time, rhosEst, 'r');
xlabel('time');
ylabel('roc');

end