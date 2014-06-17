% $Id: rufSeedsAreSegmented.m,v 1.1 2006/09/01 01:46:15 anton Exp $

% check if the seeds are segmented 
function seedsAreSegmented = rufSeedsAreSegmented(directory, image)
    [pathstr, imagePrefix] = fileparts(image);
    seedsFile = length(dir(strcat(directory, '/Seed_Segmentation/Seeds_', imagePrefix, '.txt')));
    seedsAreSegmented = (seedsFile == 1);