% $Id: rufIsSegmented.m,v 1.2 2006/09/01 01:44:55 anton Exp $

% check if an image has been segmented for root, patient, batch and image
function isSegmented = rufIsSegmented(directory, image)
    [pathstr, imagePrefix] = fileparts(image);
    seedsFile = length(dir(strcat(directory, '/Seed_Segmentation/Seeds_', imagePrefix, '.txt')));
    BBsFile = length(dir(strcat(directory, '/FTRAC_Segmentation/BBs_', imagePrefix, '.txt')));
    Line1_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Line1_', imagePrefix, '.txt')));
    Line2_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Line2_', imagePrefix, '.txt')));
    Line3_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Line3_', imagePrefix, '.txt')));
    Ellipse1_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Ellipse1_', imagePrefix, '.txt')));
    Ellipse2_EqFile = length(dir(strcat(directory, '/FTRAC_Segmentation/Ellipse2_', imagePrefix, '.txt')));
    isSegmented = ((seedsFile == 1) & (BBsFile == 1) & (Line1_EqFile == 1) & (Line2_EqFile == 1) & (Line3_EqFile == 1) & (Ellipse1_EqFile == 1) & (Ellipse2_EqFile == 1));