% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION:  [retractionTimes retractionParams] = findRetractionTimes(textFile,startTime)
% -------------------------------------------------------------------------
% Inputs: textFile,startTime
% Outputs: retractionTimes retractionParams
% Sub_Functions Called: find(), cellfun(), strfind(), findTime(),
% findDistanceAndVelocity()
% Synopsis:
%   This function will find all the retraction times in the experiment as
%   well as the associated distances and velocities.
% =========================================================================

% =========================================================================
% BEGIN FUNCTION: findRetractionTimes()
% =========================================================================

function [retractionTimes retractionParams] = findRetractionTimes(textFile,startTime)

dateTimeCell = textFile(find(cellfun('isempty',strfind(textFile,'Retraction'))==0));    % Generate a string with the retraction info.

if isempty(dateTimeCell)
    retractionTimes = 0;
    retractionParams = 0;
else
    for i = 1:length(dateTimeCell)                                          % For all retractions
        retractionTimes(i,:) = findTime(dateTimeCell(i))-startTime;         % find the time...
        params = findDistanceAndVelocity(dateTimeCell(i));                  % distance and velocities...
        retractionParams(i,:) = params;                                     % And save. 
    end
end

end
% =========================================================================
% END FUNCTION: findRetractionTimes()
% =========================================================================