% $Id: rufCalibrationProcessedDirectory.m,v 1.1 2006/03/20 22:39:29 anton Exp $

% create the current processed directory from root and calibration
function directory = rufCalibrationProcessedDirectory(rootDirectory, calibID)
    directory = strcat(rootDirectory, '/calibrations/', calibID, '/generated-data/');
