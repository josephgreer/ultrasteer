% Ann Majewicz
% Needle Steering
% Live Animal Study
% Winter 2010/2011
% -------------------------------------------------------------------------
% FUNCTION: params = findDistanceAndVelocity(dateTimeCell)
% -------------------------------------------------------------------------
% Inputs: dateTimeCell - cell that contains distance and velocity info
% Outputs: params - 2x1 cell of distance and velocity numbers
% Sub_Functions Called: textscan(), find(), cellfun(), strfind(), fliplr()
% Synopsis:
%   This function will find the velocity and distance associated with some
%   motion (insertion or retraction)
% =========================================================================


% =========================================================================
% BEGIN FUNCTION: findDistanceAndVelocity()
% =========================================================================

function params = findDistanceAndVelocity(dateTimeCell)

textString = textscan(char(dateTimeCell),'%s');
textString = textString{1};

distanceLine = textString(find(cellfun('isempty',strfind(textString,'Distance'))==0)); % Find the distance info
distanceToken = fliplr(strtok(fliplr(char(distanceLine)),':'));                        % Get just the number

velocityLine = textString(find(cellfun('isempty',strfind(textString,'Velocity'))==0)); % Find the velocity info
velocityToken = fliplr(strtok(fliplr(char(velocityLine)),':'));                        % Get just the number

params = {{distanceToken},{velocityToken}};                                             % Save.

end

% =========================================================================
% END FUNCTION: findDistanceAndVelocity()
% =========================================================================