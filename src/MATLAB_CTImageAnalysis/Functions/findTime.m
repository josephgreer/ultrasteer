% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: time = findTime(dateTimeCell)
% -------------------------------------------------------------------------
% Inputs: dateTimeCell - 
% Outputs: time - 
% Sub_Functions Called: fliplr(), char(), textscan()
% Synopsis:
%   This function will convert the text date time string into an integer
%   time.
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: findTime()
% =========================================================================

function time = findTime(dateTimeCell)

timeChar = fliplr(strtok(fliplr((char(dateTimeCell))),'_'));
timeCell = textscan(timeChar,'%u','delimiter','-');
timeArray = timeCell{1};                                % Extract the hours, minutes, seconds strings
time = timeArray(1)*3600+timeArray(2)*60+timeArray(3);  % Convert to a number

end
% =========================================================================
% END FUNCTION: findTime()
% =========================================================================
