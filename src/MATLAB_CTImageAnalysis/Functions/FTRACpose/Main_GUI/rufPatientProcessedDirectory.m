% $Id: rufPatientProcessedDirectory.m,v 1.1 2006/03/20 22:39:29 anton Exp $

% create the current processed directory from root and patient
function directory = rufPatientProcessedDirectory(rootDirectory, patientID)
    directory = strcat(rootDirectory, '/patients/', patientID, '/processed-data/intra-op/fluoro/video');
