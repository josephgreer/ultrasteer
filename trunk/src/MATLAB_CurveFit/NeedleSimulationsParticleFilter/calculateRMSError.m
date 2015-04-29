%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Plot particle filter results
%%% Inputs:
%%%     results: defined in runSimulation.m
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rmsError = calculateRMSError(results)

var = 0;
for i=1:length(results.states)
    delta = results.estimatedStates{i}.pos-results.states{i}.pos;
    var = var+dot(delta,delta);
end
var = sqrt(var/length(results.states));
rmsError = var;
end