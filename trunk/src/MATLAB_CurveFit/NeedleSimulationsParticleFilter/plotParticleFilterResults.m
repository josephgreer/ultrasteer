%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Plot particle filter results
%%% Inputs:
%%%     results: defined in runSimulation.m
%%%     figureNumber: figure number
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function plotParticleFilterResults(results, figureNumber)
states = cell2mat(results.states);
estimatedStates = cell2mat(results.estimatedStates);
truePos = [states.pos]';
estimatedPos = [estimatedStates.pos]';
figure(figureNumber);
coordLabels = {'x coord', 'y coord', 'z coord'};
for i=1:3
subplot(2,2,i);
hold on;
plot(results.time, estimatedPos(:,i));
plot(results.time, truePos(:,i), 'r--');
xlabel('time');
ylabel(coordLabels{i});
end
end