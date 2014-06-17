% $Id: rufCalibRawDirectory.m,v 1.2 2006/06/09 16:29:27 anton Exp $

% create the current raw directory from root and calibration
function directory = rufPatientRawDirectory(rootDirectory, sessionID)
    directory = strcat(rootDirectory, '/calibrations/', sessionID, '/raw-data/video/');
