% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: [stopTime resetFlag resetDist] = findStopTime(textFile,startTime,insertionTimes, insertionParams, retractionTimes, retractionParams)
% -------------------------------------------------------------------------
% Inputs: textFile,startTime,insertionTimes (matrix of insertion times), insertionParams 
% (matrix of insertion distances and velocities), retractionTimes (matrix 
% of retraction times), retractionParams (matrix of retraction distances 
% and velocitues),stopTime - stop time in seconds. 
% Outputs: stopTime resetFlag resetDist
% Sub_Functions Called: xx
% Synopsis:
%   This function will find the stop time - either when the slide was reset
%   to zero or when the slide stopped moving. 
% =========================================================================

% =========================================================================
% BEGIN FUNCTION: findStopTime()
% =========================================================================

function [stopTime resetFlag resetDist] = findStopTime(textFile,startTime,insertionTimes, insertionParams, retractionTimes, retractionParams)

resetDist = 0;

dateTime = textFile(find(cellfun('isempty',strfind(textFile,'Setting Slide to Zero'))==0)); % Finding the time at which the slide was reset. 
if isempty(dateTime)
    resetFlag = 0;
      [maxTime I]= max([insertionTimes(end),retractionTimes(end)]);         % If the slide was not reset, look for the when the slide stopped moving. 
        if I == 1
            dist = textscan(char(insertionParams{end,1}),'%f');
            vel = textscan(char(insertionParams{end,2}),'%f');
            stopTime = maxTime + dist{1}/vel{1};
        else
            dist = textscan(char(retractionParams{end,1}),'%f');
            vel = textscan(char(retractionParams{end,2}),'%f');
            stopTime = maxTime + dist{1}/vel{1};
        end
else
    stopTime = findTime(dateTime)-startTime;                                
    
    textString = textscan(char(dateTime),'%s');
    textString = textString{1};

    distanceLine = textString(find(cellfun('isempty',strfind(textString,'Distance'))==0));
    resetDist = fliplr(strtok(fliplr(char(distanceLine)),':'));
    resetFlag = 1;
end

end
% =========================================================================
% END FUNCTION: findStopTime()
% =========================================================================
      