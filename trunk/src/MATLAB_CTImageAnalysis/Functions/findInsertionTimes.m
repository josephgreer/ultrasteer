% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: [insertionTimes insertionParams] = findInsertionTimes(textFile,startTime)
% -------------------------------------------------------------------------
% Inputs: textFile - input text for the param file , startTime - in seconds
% Outputs: insertionTimes, insertionParams
% Sub_Functions Called: cellfun(), find(),strfind(),findTime(),findDistance
% Synopsis:
%   This function will find all the insertions (time distance and
%   velocities) during the experiment. 
% =========================================================================

% =========================================================================
% BEGIN FUNCTION: findInsertionTimes()
% =========================================================================

function [insertionTimes insertionParams] = findInsertionTimes(textFile,startTime)

dateTimeCell = textFile(find(cellfun('isempty',strfind(textFile,'Insertion'))==0));     % Make a cell with the insertion data

    if isempty(dateTimeCell)
        insertionTimes = 0;
        insertionParams = 0;
    else
        for i = 1:length(dateTimeCell)                                      % For all non-empty fields find the insertion times
            insertionTimes(i,:) = findTime(dateTimeCell(i)) - startTime;
            params = findDistanceAndVelocity(dateTimeCell(i));              % Save the distance and velocities too...
            insertionParams(i,:) = params;
        end
    end
end

% =========================================================================
% END FUNCTION: findInsertionTimes()
% =========================================================================