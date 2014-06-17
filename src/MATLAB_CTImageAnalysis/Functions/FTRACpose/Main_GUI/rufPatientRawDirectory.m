% $Id: rufPatientRawDirectory.m,v 1.5 2006/03/20 22:39:29 anton Exp $

% create the current raw directory from root and patient
function directory = rufPatientRawDirectory(rootDirectory, patientID)
    directory = strcat(rootDirectory, '/patients/', patientID, '/raw-data/intra-op/fluoro/video');
