% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: [breakStart breakStop] = findBreakStartStopTime(insertionTimes, insertionParams, retractionTimes, retractionParams,stopTime)
% -------------------------------------------------------------------------
% Inputs: insertionTimes (matrix of insertion times), insertionParams 
% (matrix of insertion distances and velocities), retractionTimes (matrix 
% of retraction times), retractionParams (matrix of retraction distances 
% and velocitues),stopTime - stop time in seconds. 
% Outputs: breakStart breakStop (in seconds)
% Sub_Functions Called: ls3dcircle()
% Synopsis:
%   This function will find the breakStart and breakStop time as defined as
%   the 5 sec after the last insertion/retraction and 5 sec before the
%   final retraction of the slide. 
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: findBreakStartStopTime()
% =========================================================================

function [breakStart breakStop] = findBreakStartStopTime(insertionTimes, insertionParams, retractionTimes, retractionParams,stopTime)

    [maxTime I]= max([insertionTimes(end),retractionTimes(end)]);          % Find 5 seconds after the last movement of the slide (to allow for settle)
        if I == 1
            dist = textscan(char(insertionParams{end,1}),'%f');
            vel = textscan(char(insertionParams{end,2}),'%f');
            breakStart = maxTime + dist{1}/vel{1}+5;
        else
            dist = textscan(char(retractionParams{end,1}),'%f');
            vel = textscan(char(retractionParams{end,2}),'%f');
            breakStart = maxTime + dist{1}/vel{1}+5;
        end
        

breakStop = stopTime - 5;                                                   % 5 sec before the slide resets (to avoid missing data during reset)


end

% =========================================================================
% END FUNCTION: findBreakStartStopTime()
% =========================================================================