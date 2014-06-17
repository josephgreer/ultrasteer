% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: startTime = findStartTime(textFile)
% -------------------------------------------------------------------------
% Inputs: textFile
% Outputs: startTime
% Sub_Functions Called: find(), cellfun(), strfind(), findTime()
% Synopsis:
%   This function will find the start time of the experiment. 
% =========================================================================

% =========================================================================
% BEGIN FUNCTION: findStartTime()
% =========================================================================

function startTime = findStartTime(textFile)

dateTime = textFile(find(cellfun('isempty',strfind(textFile,'Start_Time'))==0)); % Find the start time string
startTime = findTime(dateTime);                                             % Convert to seconds

end
% =========================================================================
% END FUNCTION: findStartTime()
% =========================================================================
